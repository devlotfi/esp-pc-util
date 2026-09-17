import {
  app,
  BrowserWindow,
  globalShortcut,
  ipcMain,
  Tray,
  Menu,
  nativeImage,
} from "electron";
import path from "path";
import { ipcDefinition, type ConnectPayload } from "../shared/ipc.ts";
import { JsonSerial } from "./json-serial.ts";
import { SerialPort } from "serialport";
import type { JsonMessage } from "../shared/types/json-message.ts";
import {
  getCpuUsagePercentage,
  getRamUsagePercentage,
} from "./system-stats.ts";

let serial: JsonSerial | null = null;
let mainWindow: BrowserWindow | null = null;
let tray: Tray | null = null;
let isQuitting = false;

function createWindow() {
  const win = new BrowserWindow({
    width: 900,
    height: 600,
    titleBarStyle: "hidden",
    webPreferences: {
      preload: path.join(import.meta.dirname, "../preload/preload.cjs"),
      nodeIntegration: false,
      contextIsolation: true,
    },
  });

  win.webContents.session.setPermissionCheckHandler(() => true);
  win.webContents.session.setDevicePermissionHandler(() => true);

  if (app.isPackaged) {
    win.loadFile(path.join(import.meta.dirname, "../renderer/index.html"));
  } else {
    win.loadURL("http://localhost:5173");
    win.webContents.openDevTools();
  }

  // Intercept close: hide instead of destroying the window, unless we're actually quitting
  win.on("close", (event) => {
    if (!isQuitting) {
      event.preventDefault();
      win.hide();
    }
  });

  return win;
}

function createTray() {
  const iconPath = path.join(import.meta.dirname, "../assets/tray-icon.png");
  const icon = nativeImage.createFromPath(iconPath);
  tray = new Tray(icon);

  const rebuildMenu = () => {
    console.log("Tray: rebuild");
    const connected = serial?.isOpen();
    tray!.setContextMenu(
      Menu.buildFromTemplate([
        {
          label: "Show Window",
          click: () => {
            mainWindow?.show();
          },
        },
        { type: "separator" },
        {
          label: connected ? "Serial: Connected" : "Serial: Disconnected",
          enabled: false,
        },
        { type: "separator" },
        {
          label: "Quit",
          click: () => {
            isQuitting = true;
            app.quit();
          },
        },
      ]),
    );
  };

  rebuildMenu();
  tray.setToolTip("ESP PC Util");
  tray.on("click", () => {
    mainWindow?.show();
  });

  return { rebuildMenu };
}

let pcStatsSendInterval: any = null;

async function sendPcStats() {
  console.log("send data");
  const cpu = await getCpuUsagePercentage();
  const ram = getRamUsagePercentage();

  if (serial) {
    await serial.send({
      type: "PC_STATS",
      cpu,
      ram,
    });
  }
}

app.whenReady().then(() => {
  mainWindow = createWindow();
  const { rebuildMenu } = createTray();

  globalShortcut.register("F5", () => {
    mainWindow?.reload();
  });

  ipcMain.handle(ipcDefinition.window.invoke.minimize, () => {
    mainWindow?.minimize();
  });

  ipcMain.handle(ipcDefinition.window.invoke.maximize, () => {
    if (!mainWindow) return;
    if (mainWindow.isMaximized()) {
      mainWindow.unmaximize();
    } else {
      mainWindow.maximize();
    }
  });

  // "Close" from the titlebar now just hides the window (tray keeps it alive)
  ipcMain.handle(ipcDefinition.window.invoke.close, () => {
    mainWindow?.hide();
  });

  ipcMain.handle(ipcDefinition.espPcUtil.invoke.listPorts, async () => {
    const ports = await SerialPort.list();
    return ports;
  });

  ipcMain.handle(
    ipcDefinition.espPcUtil.invoke.sendJson,
    (_, json: JsonMessage) => {
      if (serial) {
        serial.send(json);
      }
    },
  );

  ipcMain.handle(
    ipcDefinition.espPcUtil.invoke.connect,
    async (_, payload: ConnectPayload) => {
      if (serial) {
        await serial.close();
      }

      serial = new JsonSerial({
        path: payload.port,
        baudRate: payload.baudRate,
      });

      serial.onConnected(() => {
        mainWindow?.webContents.send(
          ipcDefinition.espPcUtil.events.mainToRenderer.connected,
        );
        if (pcStatsSendInterval !== null) {
          clearInterval(pcStatsSendInterval);
          pcStatsSendInterval = null;
        } else {
          pcStatsSendInterval = setInterval(sendPcStats, 2000);
        }
        rebuildMenu();
      });

      serial.onDisconnected(() => {
        mainWindow?.webContents.send(
          ipcDefinition.espPcUtil.events.mainToRenderer.closed,
        );
        if (pcStatsSendInterval) {
          clearInterval(pcStatsSendInterval);
          pcStatsSendInterval = null;
        }
        rebuildMenu();
      });

      serial.onError((error) => {
        mainWindow?.webContents.send(
          ipcDefinition.espPcUtil.events.mainToRenderer.error,
          { message: error.message },
        );
      });

      serial.onJson((json) => {
        if (typeof json === "object" && json !== null && "type" in json) {
          mainWindow?.webContents.send(
            ipcDefinition.espPcUtil.events.mainToRenderer.json,
            json,
          );
        }
      });
    },
  );

  ipcMain.handle(ipcDefinition.espPcUtil.invoke.close, async () => {
    if (serial) {
      await serial.close();
      serial = null;
      rebuildMenu();
    }
  });

  app.on("activate", () => {
    if (BrowserWindow.getAllWindows().length === 0) {
      mainWindow = createWindow();
    } else {
      mainWindow?.show();
    }
  });
});

// Don't quit when all windows are closed — the tray keeps the process alive
app.on("window-all-closed", () => {});

// Make sure the serial port is closed cleanly when the app actually quits
app.on("before-quit", async (event) => {
  isQuitting = true;
  if (serial) {
    event.preventDefault();
    await serial.close();
    serial = null;
    app.quit();
  }
});
