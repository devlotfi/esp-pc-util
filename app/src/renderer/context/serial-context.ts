import { createContext } from "react";
import type { ConnectPayload } from "../../shared/ipc";

interface SerialContext {
  connected: boolean;
  connectionInfo: ConnectPayload | null;
  setConnectionInfo: (value: ConnectPayload | null) => void;
}

export const SerialContextInitialValue: SerialContext = {
  connected: false,
  connectionInfo: null,
  setConnectionInfo() {},
};

export const SerialContext = createContext(SerialContextInitialValue);
