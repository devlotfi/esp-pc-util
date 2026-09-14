import { contextBridge, ipcRenderer } from "electron";
import { ipcDefinition } from "../shared/ipc.ts";

contextBridge.exposeInMainWorld("electronAPI", {
  window: {
    minimize: () => ipcRenderer.invoke(ipcDefinition.window.minimize),
    maximize: () => ipcRenderer.invoke(ipcDefinition.window.maximize),
    close: () => ipcRenderer.invoke(ipcDefinition.window.close),
  },
});
