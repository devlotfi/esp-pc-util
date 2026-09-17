import type { BaudRate } from "../../shared/ipc";

export interface SerialConnection {
  port: string;
  baudRate: BaudRate;
}
