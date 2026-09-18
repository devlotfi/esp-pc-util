import { app, BrowserWindow, globalShortcut, ipcMain } from "electron";
import { ipcDefinition, type ConnectPayload } from "../shared/ipc.ts";
import { JsonSerial } from "./json-serial.ts";
import { SerialPort } from "serialport";
import type { JsonMessage } from "../shared/types/json-message.ts";
import { createWindow } from "./create-window.ts";
import { createTray } from "./create-tray.ts";
import { Vars } from "./vars.ts";
import { sendPcStats } from "./system-stats.ts";
import { showMainWindow } from "./show-main-window.ts";

export function appSetup() {
  app.setLoginItemSettings({
    openAtLogin: true,
    path: process.execPath,
    args: ["--hidden"],
  });

  app.on("second-instance", () => {
    // Someone tried to open a second instance — focus/show the existing one instead
    if (Vars.mainWindow) {
      if (Vars.mainWindow.isMinimized()) Vars.mainWindow.restore();
      Vars.mainWindow.setSkipTaskbar(false);
      Vars.mainWindow.show();
      Vars.mainWindow.focus();
    }
  });

  app.whenReady().then(() => {
    Vars.mainWindow = createWindow();
    const { rebuildMenu } = createTray();

    globalShortcut.register("F5", () => {
      Vars.mainWindow?.reload();
    });

    ipcMain.handle(ipcDefinition.window.invoke.minimize, () => {
      Vars.mainWindow?.minimize();
    });

    ipcMain.handle(ipcDefinition.window.invoke.maximize, () => {
      if (!Vars.mainWindow) return;
      if (Vars.mainWindow.isMaximized()) {
        Vars.mainWindow.unmaximize();
      } else {
        Vars.mainWindow.maximize();
      }
    });

    // "Close" from the titlebar now just hides the window (tray keeps it alive)
    ipcMain.handle(ipcDefinition.window.invoke.close, () => {
      Vars.mainWindow?.hide();
    });

    ipcMain.handle(ipcDefinition.espPcUtil.invoke.listPorts, async () => {
      const ports = await SerialPort.list();
      return ports;
    });

    ipcMain.handle(
      ipcDefinition.espPcUtil.invoke.sendJson,
      (_, json: JsonMessage) => {
        if (Vars.serial) {
          Vars.serial.send(json);
        }
      },
    );

    ipcMain.handle(
      ipcDefinition.espPcUtil.invoke.connect,
      async (_, payload: ConnectPayload) => {
        if (Vars.serial) {
          await Vars.serial.close();
        }

        Vars.serial = new JsonSerial({
          path: payload.port,
          baudRate: payload.baudRate,
        });

        Vars.serial.onConnected(() => {
          Vars.mainWindow?.webContents.send(
            ipcDefinition.espPcUtil.events.mainToRenderer.connected,
          );
          if (Vars.pcStatsSendInterval !== null) {
            clearInterval(Vars.pcStatsSendInterval);
          }
          Vars.pcStatsSendInterval = setInterval(sendPcStats, 2000);
          rebuildMenu();
        });

        Vars.serial.onDisconnected(() => {
          Vars.mainWindow?.webContents.send(
            ipcDefinition.espPcUtil.events.mainToRenderer.closed,
          );
          if (Vars.pcStatsSendInterval) {
            clearInterval(Vars.pcStatsSendInterval);
            Vars.pcStatsSendInterval = null;
          }
          rebuildMenu();
        });

        Vars.serial.onError((error) => {
          Vars.mainWindow?.webContents.send(
            ipcDefinition.espPcUtil.events.mainToRenderer.error,
            { message: error.message },
          );
        });

        Vars.serial.onJson((json) => {
          if (typeof json === "object" && json !== null && "type" in json) {
            Vars.mainWindow?.webContents.send(
              ipcDefinition.espPcUtil.events.mainToRenderer.json,
              json,
            );
          }
        });
      },
    );

    ipcMain.handle(ipcDefinition.espPcUtil.invoke.close, async () => {
      if (Vars.serial) {
        await Vars.serial.close();
        Vars.serial = null;
        rebuildMenu();
      }
    });

    app.on("activate", () => {
      if (BrowserWindow.getAllWindows().length === 0) {
        Vars.mainWindow = createWindow();
      } else {
        showMainWindow();
      }
    });
  });

  // Don't quit when all windows are closed — the tray keeps the process alive
  app.on("window-all-closed", () => {});

  // Make sure the serial port is closed cleanly when the app actually quits
  app.on("before-quit", async (event) => {
    Vars.isQuitting = true;
    if (Vars.serial) {
      event.preventDefault();
      if (Vars.pcStatsSendInterval !== null) {
        clearInterval(Vars.pcStatsSendInterval);
        Vars.pcStatsSendInterval = null;
      }
      await Vars.serial.close();
      Vars.serial = null;
      app.quit();
    }
  });

  app.on("will-quit", () => {
    globalShortcut.unregisterAll();
    Vars.tray?.destroy();
  });
}
