import { createContext } from "react";

interface SerialContext {
  connected: boolean;
}

export const SerialContextInitialValue: SerialContext = {
  connected: false,
};

export const SerialContext = createContext(SerialContextInitialValue);
