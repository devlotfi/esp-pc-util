import { createContext, createRef, type RefObject } from "react";

interface AppContext {
  scrollRef: RefObject<HTMLDivElement | null>;
  sidebarOpen: boolean;
  setSidebarOpen: (value: boolean) => void;
}

export const AppContextInitialValue: AppContext = {
  scrollRef: createRef(),
  sidebarOpen: true,
  setSidebarOpen() {},
};

export const AppContext = createContext(AppContextInitialValue);
