import { app, BrowserWindow, globalShortcut, ipcMain } from "electron";
import path from "path";
import { ipcDefinition } from "../shared/ipc.ts";
import { SerialPort } from "serialport";

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

  ipcMain.handle(ipcDefinition.window.minimize, () => {
    mainWindow.minimize();
  });
  ipcMain.handle(ipcDefinition.window.maximize, () => {
    if (mainWindow.isMaximized()) {
      mainWindow.unmaximize();
    } else {
      mainWindow.maximize();
    }
  });
  ipcMain.handle(ipcDefinition.window.close, () => {
    mainWindow.close();
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

const port = new SerialPort({
  path: "COM3",
  baudRate: 115200,
});

port.on("error", function (err) {
  console.log("Error: ", err.message);
});

port.on("data", function (data: Buffer) {
  console.log("Data:", data.toString("utf-8"));
});
