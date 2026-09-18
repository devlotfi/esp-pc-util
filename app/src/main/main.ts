import { app } from "electron";
import { Vars } from "./vars.ts";
import { appSetup } from "./app-setup.ts";

async function main() {
  if (!Vars.gotLock) {
    app.quit();
    return;
  }
  appSetup();
}
main();
