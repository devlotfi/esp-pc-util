import type { electronApi } from "../preload/preload";

export {}; // Ensure this file is treated as a module

declare global {
  interface Window {
    electronAPI: typeof electronApi;
  }
}
