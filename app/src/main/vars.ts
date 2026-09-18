import { app, BrowserWindow, Tray } from "electron";
import { JsonSerial } from "./json-serial.ts";

export abstract class Vars {
  static gotLock = app.requestSingleInstanceLock();
  static startHidden = process.argv.includes("--hidden");
  static serial: JsonSerial | null = null;
  static mainWindow: BrowserWindow | null = null;
  static tray: Tray | null = null;
  static isQuitting = false;
  static pcStatsSendInterval: NodeJS.Timeout | null = null;
}
