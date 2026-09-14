import { useState, type PropsWithChildren } from "react";
import { AppContext, AppContextInitialValue } from "../context/app-context";

export default function AppProvider({ children }: PropsWithChildren) {
  const [sidebarOpen, setSidebarOpen] = useState<boolean>(
    AppContextInitialValue.sidebarOpen,
  );

  return (
    <AppContext.Provider
      value={{
        scrollRef: AppContextInitialValue.scrollRef,
        sidebarOpen,
        setSidebarOpen,
      }}
    >
      {children}
    </AppContext.Provider>
  );
}
