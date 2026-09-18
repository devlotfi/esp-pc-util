import { app, Tray, Menu, nativeImage } from "electron";
import path from "path";
import { Vars } from "./vars.ts";
import { showMainWindow } from "./show-main-window.ts";

export function createTray() {
  const iconPath = path.join(import.meta.dirname, "../assets/tray-icon.png");
  const icon = nativeImage.createFromPath(iconPath);
  Vars.tray = new Tray(icon);

  const rebuildMenu = () => {
    console.log("Tray: rebuild");
    const connected = Vars.serial?.isOpen();
    Vars.tray!.setContextMenu(
      Menu.buildFromTemplate([
        {
          label: "Show Window",
          click: () => {
            showMainWindow();
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
            Vars.isQuitting = true;
            app.quit();
          },
        },
      ]),
    );
  };

  rebuildMenu();
  Vars.tray.setToolTip("ESP PC Util");
  Vars.tray.on("click", () => {
    showMainWindow();
  });

  return { rebuildMenu };
}
