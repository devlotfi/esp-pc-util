import * as React from "react";
import { Outlet, createRootRoute } from "@tanstack/react-router";
import RouteLoading from "../components/route-loading";
import Titlebar from "../components/titlebar";

export const Route = createRootRoute({
  component: RootComponent,
});

function RootComponent() {
  return (
    <React.Fragment>
      <RouteLoading></RouteLoading>
      <Titlebar></Titlebar>
      <Outlet />
    </React.Fragment>
  );
}
