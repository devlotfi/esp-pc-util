interface SetLedMessage {
  type: "SET_LED";
  color: string;
  brightness: number;
}

export type JsonMessage = SetLedMessage;
