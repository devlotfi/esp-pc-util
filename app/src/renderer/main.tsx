import "./i18n.ts";
import { StrictMode } from "react";
import { createRoot } from "react-dom/client";
import "./index.css";
import {
  RouterProvider,
  createHashHistory,
  createRouter,
} from "@tanstack/react-router";
import { QueryClient, QueryClientProvider } from "@tanstack/react-query";
import { routeTree } from "./routeTree.gen.ts";
import { ThemeProvider } from "./provider/theme-provider.tsx";
import AppProvider from "./provider/app-provider.tsx";
import NotFound from "./components/not-found.tsx";

const history = createHashHistory();

const router = createRouter({
  routeTree,
  history,
  defaultNotFoundComponent: NotFound,
});

declare module "@tanstack/react-router" {
  interface Register {
    router: typeof router;
  }
}

const queryClient = new QueryClient();

createRoot(document.getElementById("root")!).render(
  <StrictMode>
    <div className="flex flex-col min-h-dvh min-w-dvw max-h-dvh max-w-dvw overflow-hidden bg-background">
      <QueryClientProvider client={queryClient}>
        <ThemeProvider>
          <AppProvider>
            <RouterProvider router={router}></RouterProvider>
          </AppProvider>
        </ThemeProvider>
      </QueryClientProvider>
    </div>
  </StrictMode>,
);
