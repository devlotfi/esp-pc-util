import { app, BrowserWindow } from "electron";
import path from "path";
import { Vars } from "./vars.ts";

export function createWindow() {
  const win = new BrowserWindow({
    width: 900,
    height: 600,
    titleBarStyle: "hidden",
    show: false,
    skipTaskbar: Vars.startHidden,
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
    if (!Vars.startHidden) {
      win.webContents.openDevTools();
    }
  }

  win.once("ready-to-show", () => {
    if (!Vars.startHidden) {
      win.show();
    }
  });

  // Intercept close: hide instead of destroying the window, unless we're actually quitting
  win.on("close", (event) => {
    if (!Vars.isQuitting) {
      event.preventDefault();
      win.hide();
    }
  });

  return win;
}
