import { Vars } from "./vars.ts";

export function showMainWindow() {
  if (!Vars.mainWindow) return;
  Vars.mainWindow.setSkipTaskbar(false);
  Vars.mainWindow.show();
  Vars.mainWindow.focus();
}
