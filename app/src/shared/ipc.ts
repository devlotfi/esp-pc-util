import type { PortInfo } from "./types/port-info";

export const ipcDefinition = {
  window: {
    minimize: "invoke/window/minimize",
    maximize: "invoke/window/maximize",
    close: "invoke/window/close",
  },
  espPcUtil: {
    listPorts: "invoke/esp-pc-util/list-ports",
    connect: "invoke/esp-pc-util/connect",
    close: "invoke/esp-pc-util/close",
    sendJson: "invoke/esp-pc-util/send-json",
    connected: "event/esp-pc-util/connected",
    closed: "event/esp-pc-util/closed",
    error: "event/esp-pc-util/error",
    json: "event/esp-pc-util/json",
    setLed: "invoke/esp-pc-util/set-led",
  },
} as const;

export interface ElectronAPI {
  window: {
    minimize(): Promise<void>;
    maximize(): Promise<void>;
    close(): Promise<void>;
  };
  espPcUtil: {
    listPorts(): Promise<PortInfo>;
    setLed(payload: SetLedPayload): Promise<void>;
    connect(payload: ConnectPayload): Promise<void>;
    close(): Promise<void>;
    sendJson<T = any>(json: T): Promise<void>;
    onConnected(callback: () => void): () => void;
    onDisconnected(callback: () => void): () => void;
    onError(callback: (error: { message: string }) => void): () => void;
    onJson(callback: (json: unknown) => void): () => void;
  };
}

export type BaudRate =
  | 110
  | 300
  | 600
  | 1200
  | 2400
  | 4800
  | 9600
  | 14400
  | 19200
  | 28800
  | 38400
  | 56000
  | 57600
  | 115200
  | 128000
  | 230400
  | 256000
  | 460800
  | 500000
  | 576000
  | 921600
  | 1000000
  | 1152000
  | 1500000
  | 2000000
  | 2500000
  | 3000000
  | 4000000;

export interface SetLedPayload {
  color: string;
  brightness: number;
}

export interface ConnectPayload {
  port: string;
  baudRate: BaudRate;
}
