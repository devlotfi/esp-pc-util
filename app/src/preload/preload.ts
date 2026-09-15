import { contextBridge, ipcRenderer } from "electron";
import {
  ipcDefinition,
  type ConnectPayload,
  type ElectronAPI,
  type SetLedPayload,
} from "../shared/ipc.ts";

contextBridge.exposeInMainWorld("electronAPI", {
  window: {
    minimize: () => ipcRenderer.invoke(ipcDefinition.window.minimize),
    maximize: () => ipcRenderer.invoke(ipcDefinition.window.maximize),
    close: () => ipcRenderer.invoke(ipcDefinition.window.close),
  },
  espPcUtil: {
    setLed: (payload: SetLedPayload) =>
      ipcRenderer.invoke(ipcDefinition.espPcUtil.setLed, payload),
    listPorts: () => ipcRenderer.invoke(ipcDefinition.espPcUtil.listPorts),
    connect: (payload: ConnectPayload) =>
      ipcRenderer.invoke(ipcDefinition.espPcUtil.connect, payload),
    close: () => ipcRenderer.invoke(ipcDefinition.espPcUtil.close),
    sendJson: (json: any) =>
      ipcRenderer.invoke(ipcDefinition.espPcUtil.sendJson, json),
    onConnected(callback: () => void) {
      const listener = () => callback();
      ipcRenderer.on(ipcDefinition.espPcUtil.connected, listener);
      return () => {
        ipcRenderer.removeListener(ipcDefinition.espPcUtil.connected, listener);
      };
    },
    onDisconnected(callback: () => void) {
      const listener = () => callback();
      ipcRenderer.on(ipcDefinition.espPcUtil.closed, listener);
      return () => {
        ipcRenderer.removeListener(ipcDefinition.espPcUtil.closed, listener);
      };
    },
    onError(callback: (error: { message: string }) => void) {
      const listener = (
        _event: Electron.IpcRendererEvent,
        error: { message: string },
      ) => {
        callback(error);
      };
      ipcRenderer.on(ipcDefinition.espPcUtil.error, listener);
      return () => {
        ipcRenderer.removeListener(ipcDefinition.espPcUtil.error, listener);
      };
    },
    onJson(callback: (json: unknown) => void) {
      const listener = (_event: Electron.IpcRendererEvent, json: unknown) => {
        callback(json);
      };
      ipcRenderer.on(ipcDefinition.espPcUtil.json, listener);
      return () => {
        ipcRenderer.removeListener(ipcDefinition.espPcUtil.json, listener);
      };
    },
  },
} satisfies ElectronAPI);
