import type { ElectronAPI } from "../shared/ipc";

export {}; // Ensure this file is treated as a module

declare global {
  interface Window {
    electronAPI: ElectronAPI;
  }
}
