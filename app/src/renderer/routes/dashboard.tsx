import { createFileRoute, Navigate } from "@tanstack/react-router";
import { useContext } from "react";
import { SerialContext } from "../context/serial-context";
import DisplaySettings from "../components/dashboard/display-settings";
import LedSettings from "../components/dashboard/led-settings";
import WallpaperSettings from "../components/dashboard/wallpaper-settings";
import { ScrollShadow } from "@heroui/react";

export const Route = createFileRoute("/dashboard")({
  component: RouteComponent,
});

function RouteComponent() {
  const { connected } = useContext(SerialContext);

  if (!connected) return <Navigate to="/"></Navigate>;

  return (
    <ScrollShadow className="flex flex-col flex-1 items-center h-[calc(100dvh-3.5rem)] overflow-x-hidden overflow-y-auto">
      <div className="flex flex-col p-[1rem] w-full max-w-screen-lg">
        <WallpaperSettings></WallpaperSettings>
      </div>
      <div className="flex flex-col lg:flex-row w-full max-w-screen-lg p-[1rem] gap-[1rem]">
        <div className="flex flex-col flex-1 gap-[1rem]">
          <DisplaySettings></DisplaySettings>
        </div>
        <div className="flex flex-col flex-1 gap-[1rem] ">
          <LedSettings></LedSettings>
        </div>
      </div>
    </ScrollShadow>
  );
}
