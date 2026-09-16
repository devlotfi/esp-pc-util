import { createFileRoute, Navigate } from "@tanstack/react-router";
import { useContext } from "react";
import { useTranslation } from "react-i18next";
import { SerialContext } from "../context/serial-context";
import DisplaySettings from "../components/dashboard/display-settings";
import LedSettings from "../components/dashboard/led-settings";

export const Route = createFileRoute("/dashboard")({
  component: RouteComponent,
});

function RouteComponent() {
  const { t } = useTranslation();
  const { connected } = useContext(SerialContext);

  if (!connected) return <Navigate to="/"></Navigate>;

  return (
    <div className="flex flex-col flex-1 items-center h-[calc(100dvh-4.5rem)] overflow-x-hidden overflow-y-auto">
      <div className="flex flex-col lg:flex-row w-full max-w-screen-lg p-[1rem] gap-[1rem]">
        <div className="flex flex-col flex-1 gap-[1rem]">
          <DisplaySettings></DisplaySettings>
        </div>
        <div className="flex flex-col flex-1 gap-[1rem] ">
          <LedSettings></LedSettings>
        </div>
      </div>
    </div>
  );
}
