import { contextBridge, ipcRenderer } from "electron";
import { ipcDefinition, type ConnectPayload } from "../shared/ipc.ts";
import type { JsonMessage } from "../shared/types/json-message.ts";
import type { PortInfo } from "../shared/types/port-info.ts";

export const electronApi = {
  window: {
    minimize: () => ipcRenderer.invoke(ipcDefinition.window.invoke.minimize),
    maximize: () => ipcRenderer.invoke(ipcDefinition.window.invoke.maximize),
    close: () => ipcRenderer.invoke(ipcDefinition.window.invoke.close),
  },
  espPcUtil: {
    listPorts: (): Promise<PortInfo[]> =>
      ipcRenderer.invoke(ipcDefinition.espPcUtil.invoke.listPorts),
    connect: (payload: ConnectPayload) =>
      ipcRenderer.invoke(ipcDefinition.espPcUtil.invoke.connect, payload),
    close: () => ipcRenderer.invoke(ipcDefinition.espPcUtil.invoke.close),
    sendJson: (json: JsonMessage) =>
      ipcRenderer.invoke(ipcDefinition.espPcUtil.invoke.sendJson, json),
    onConnected(callback: () => void) {
      const listener = () => callback();
      ipcRenderer.on(
        ipcDefinition.espPcUtil.events.mainToRenderer.connected,
        listener,
      );
      return () => {
        ipcRenderer.removeListener(
          ipcDefinition.espPcUtil.events.mainToRenderer.connected,
          listener,
        );
      };
    },
    onDisconnected(callback: () => void) {
      const listener = () => callback();
      ipcRenderer.on(
        ipcDefinition.espPcUtil.events.mainToRenderer.closed,
        listener,
      );
      return () => {
        ipcRenderer.removeListener(
          ipcDefinition.espPcUtil.events.mainToRenderer.closed,
          listener,
        );
      };
    },
    onError(callback: (error: { message: string }) => void) {
      const listener = (
        _event: Electron.IpcRendererEvent,
        error: { message: string },
      ) => {
        callback(error);
      };
      ipcRenderer.on(
        ipcDefinition.espPcUtil.events.mainToRenderer.error,
        listener,
      );
      return () => {
        ipcRenderer.removeListener(
          ipcDefinition.espPcUtil.events.mainToRenderer.error,
          listener,
        );
      };
    },
    onJson(callback: (json: JsonMessage) => void) {
      const listener = (
        _event: Electron.IpcRendererEvent,
        json: JsonMessage,
      ) => {
        callback(json);
      };
      ipcRenderer.on(
        ipcDefinition.espPcUtil.events.mainToRenderer.json,
        listener,
      );
      return () => {
        ipcRenderer.removeListener(
          ipcDefinition.espPcUtil.events.mainToRenderer.json,
          listener,
        );
      };
    },
  },
};

contextBridge.exposeInMainWorld("electronAPI", electronApi);
