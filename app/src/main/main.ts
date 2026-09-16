import { app, BrowserWindow, globalShortcut, ipcMain } from "electron";
import path from "path";
import { ipcDefinition, type ConnectPayload } from "../shared/ipc.ts";
import { JsonSerial } from "./json-serial.ts";
import { SerialPort } from "serialport";
import type { JsonMessage } from "../shared/types/json-message.ts";

let serial: JsonSerial | null = null;

// Create a reference for the window so that it can be accessed later
// Function to create the main window
function createWindow() {
  const mainWindow = new BrowserWindow({
    width: 900,
    height: 600,
    titleBarStyle: "hidden",
    webPreferences: {
      preload: path.join(import.meta.dirname, "../preload/preload.cjs"),
      nodeIntegration: false, // Prevents access to Node.js features from the renderer
      contextIsolation: true, // Isolates context between main and renderer process
    },
  });

  mainWindow.webContents.session.setPermissionCheckHandler(() => {
    return true;
  });

  mainWindow.webContents.session.setDevicePermissionHandler(() => {
    return true;
  });

  // Open DevTools in development mode
  if (app.isPackaged) {
    mainWindow.loadFile(
      path.join(import.meta.dirname, "../renderer/index.html"),
    );
  } else {
    mainWindow.loadURL("http://localhost:5173");
    mainWindow.webContents.openDevTools();
  }

  return mainWindow;
}

// Event listener when Electron finishes initialization
app.whenReady().then(() => {
  const mainWindow = createWindow();

  globalShortcut.register("F5", () => {
    if (mainWindow) mainWindow.reload();
  });

  ipcMain.handle(ipcDefinition.window.invoke.minimize, () => {
    mainWindow.minimize();
  });

  ipcMain.handle(ipcDefinition.window.invoke.maximize, () => {
    if (mainWindow.isMaximized()) {
      mainWindow.unmaximize();
    } else {
      mainWindow.maximize();
    }
  });

  ipcMain.handle(ipcDefinition.window.invoke.close, () => {
    mainWindow.close();
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
        mainWindow.webContents.send(
          ipcDefinition.espPcUtil.events.mainToRenderer.connected,
        );
      });

      serial.onDisconnected(() => {
        mainWindow.webContents.send(
          ipcDefinition.espPcUtil.events.mainToRenderer.closed,
        );
      });

      serial.onError((error) => {
        mainWindow.webContents.send(
          ipcDefinition.espPcUtil.events.mainToRenderer.error,
          {
            message: error.message,
          },
        );
      });

      serial.onJson((json) => {
        console.log("Received valid JSON:", json);
        if (typeof json === "object" && json !== null && "type" in json) {
          console.log("Message type:", json.type);
          mainWindow.webContents.send(
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
    }
  });

  // For macOS, create a window when the app is clicked if no other windows are open
  app.on("activate", () => {
    if (BrowserWindow.getAllWindows().length === 0) {
      createWindow();
    }
  });
});

// Event listener when all windows are closed (for Windows/Linux)
app.on("window-all-closed", () => {
  if (process.platform !== "darwin") {
    app.quit();
  }
});
