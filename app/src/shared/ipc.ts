export const ipcDefinition = {
  window: {
    minimize: "window:minimize",
    maximize: "window:maximize",
    close: "window:close",
  },
} as const;

export interface ElectronAPI {
  window: {
    minimize(): Promise<void>;
    maximize(): Promise<void>;
    close(): Promise<void>;
  };
}
