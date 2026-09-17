import { SerialPort } from "serialport";
import type { JsonMessage } from "../shared/types/json-message.ts";

export type JsonCallback<T = unknown> = (json: T) => void;
export type ConnectedCallback = () => void;
export type DisconnectedCallback = () => void;
export type ErrorCallback = (error: Error) => void;

export interface JsonSerialOptions {
  path: string;
  baudRate?: number;
}

export class JsonSerial {
  private static readonly HEADER_SIZE = 4;

  private readonly port: SerialPort;

  private rxBuffer: Buffer = Buffer.alloc(0);
  private expectedLength: number | null = null;

  private jsonCallback: JsonCallback | null = null;
  private connectedCallback: ConnectedCallback | null = null;
  private disconnectedCallback: DisconnectedCallback | null = null;
  private errorCallback: ErrorCallback | null = null;

  constructor(options: JsonSerialOptions) {
    this.port = new SerialPort({
      path: options.path,
      baudRate: options.baudRate ?? 921600,
    });

    this.port.on("open", () => {
      this.connectedCallback?.();
    });

    this.port.on("close", () => {
      this.disconnectedCallback?.();
    });

    this.port.on("data", (data: Buffer) => {
      this.handleData(data);
    });

    this.port.on("error", (error) => {
      console.error("Serial error:", error);
      this.errorCallback?.(error);
    });
  }

  onJson<T = unknown>(callback: JsonCallback<T>): void {
    this.jsonCallback = callback as JsonCallback;
  }

  onConnected(callback: ConnectedCallback): void {
    this.connectedCallback = callback;
  }

  onDisconnected(callback: DisconnectedCallback): void {
    this.disconnectedCallback = callback;
  }

  onError(callback: ErrorCallback): void {
    this.errorCallback = callback;
  }

  async send(json: JsonMessage): Promise<void> {
    const payload = Buffer.from(JSON.stringify(json), "utf8");

    if (payload.length > 0xffffffff) {
      throw new Error("JSON payload is too large");
    }

    const frame = Buffer.alloc(JsonSerial.HEADER_SIZE + payload.length);

    frame.writeUInt32BE(payload.length, 0);
    payload.copy(frame, JsonSerial.HEADER_SIZE);

    await this.write(frame);
  }

  async sendJson(json: string): Promise<void> {
    const payload = Buffer.from(json, "utf8");

    if (payload.length > 0xffffffff) {
      throw new Error("JSON payload is too large");
    }

    const frame = Buffer.alloc(JsonSerial.HEADER_SIZE + payload.length);

    frame.writeUInt32BE(payload.length, 0);
    payload.copy(frame, JsonSerial.HEADER_SIZE);

    await this.write(frame);
  }

  async close(): Promise<void> {
    if (!this.port.isOpen) {
      return;
    }

    await new Promise<void>((resolve, reject) => {
      this.port.close((error) => {
        if (error) {
          reject(error);
          return;
        }

        resolve();
      });
    });
  }

  isOpen(): boolean {
    return this.port.isOpen;
  }

  private async write(data: Buffer): Promise<void> {
    await new Promise<void>((resolve, reject) => {
      this.port.write(data, (error) => {
        if (error) {
          reject(error);
          return;
        }

        this.port.drain((error) => {
          if (error) {
            reject(error);
            return;
          }

          resolve();
        });
      });
    });
  }

  private handleData(data: Buffer): void {
    this.rxBuffer = Buffer.concat([this.rxBuffer, data]);

    while (true) {
      if (this.expectedLength === null) {
        if (this.rxBuffer.length < JsonSerial.HEADER_SIZE) {
          return;
        }

        this.expectedLength = this.rxBuffer.readUInt32BE(0);
        this.rxBuffer = this.rxBuffer.subarray(JsonSerial.HEADER_SIZE);
      }

      if (this.rxBuffer.length < this.expectedLength) {
        return;
      }

      const payload = this.rxBuffer.subarray(0, this.expectedLength);

      this.rxBuffer = this.rxBuffer.subarray(this.expectedLength);

      this.expectedLength = null;

      this.handleJson(payload);
    }
  }

  private handleJson(payload: Buffer): void {
    let json: unknown;

    try {
      json = JSON.parse(payload.toString("utf8"));
    } catch (error) {
      console.error("Invalid JSON received:", error);

      this.errorCallback?.(
        error instanceof Error ? error : new Error(String(error)),
      );

      return;
    }

    this.jsonCallback?.(json);
  }
}
