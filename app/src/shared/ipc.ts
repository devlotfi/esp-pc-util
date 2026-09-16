export const ipcDefinition = {
  window: {
    invoke: {
      minimize: "window/minimize",
      maximize: "window/maximize",
      close: "window/close",
    },
  },
  espPcUtil: {
    invoke: {
      listPorts: "esp-pc-util/list-ports",
      connect: "esp-pc-util/connect",
      close: "esp-pc-util/close",
      sendJson: "esp-pc-util/send-json",
      setLed: "esp-pc-util/set-led",
    },
    events: {
      mainToRenderer: {
        connected: "esp-pc-util/connected",
        closed: "esp-pc-util/closed",
        error: "esp-pc-util/error",
        json: "esp-pc-util/json",
      },
    },
  },
} as const;

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

export interface ConnectPayload {
  port: string;
  baudRate: BaudRate;
}
