import type { ConnectPayload } from "../../shared/ipc";
import { Constants } from "../constants";
export function loadSerialConnection(): ConnectPayload | null {
  const raw = localStorage.getItem(Constants.SERIAL_CONNECTION_STORAGE_KEY);
  if (!raw) return null;

  try {
    const serialConnection: ConnectPayload = JSON.parse(raw);
    return serialConnection;
  } catch (error) {
    return null;
  }
}

export function saveSerialConnection(serialConnection: ConnectPayload) {
  localStorage.setItem(
    Constants.SERIAL_CONNECTION_STORAGE_KEY,
    JSON.stringify(serialConnection),
  );
}

export function clearSerialConnection() {
  localStorage.removeItem(Constants.SERIAL_CONNECTION_STORAGE_KEY);
}
