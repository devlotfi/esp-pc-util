import { Button } from "@heroui/react";
import { createFileRoute } from "@tanstack/react-router";
import { useEffect } from "react";

export const Route = createFileRoute("/")({
  component: RouteComponent,
});

function RouteComponent() {
  useEffect(() => {
    const unsubscribe = window.electronAPI.espPcUtil.onConnected(() => {
      console.log("ESP32 connected");
    });
    return unsubscribe;
  }, []);
  useEffect(() => {
    const unsubscribe = window.electronAPI.espPcUtil.onDisconnected(() => {
      console.log("ESP32 disconnected");
    });
    return unsubscribe;
  }, []);
  useEffect(() => {
    const unsubscribe = window.electronAPI.espPcUtil.onError((error) => {
      console.log("error", error);
    });
    return unsubscribe;
  }, []);
  useEffect(() => {
    const unsubscribe = window.electronAPI.espPcUtil.onJson((json) => {
      console.log("json", json);
    });
    return unsubscribe;
  }, []);

  return (
    <div className="flex flex-1 justify-center items-center">
      <Button
        onPress={() =>
          window.electronAPI.espPcUtil.setLed({
            color: "#ff0000",
            brightness: 128,
          })
        }
      >
        lol1
      </Button>
      <Button
        onPress={() =>
          window.electronAPI.espPcUtil.setLed({
            color: "#00ff00",
            brightness: 128,
          })
        }
      >
        lol2
      </Button>
      <Button
        onPress={async () => {
          const res = await window.electronAPI.espPcUtil.listPorts();
          console.log(res);
        }}
      >
        lol3
      </Button>
      <Button
        onPress={async () => {
          await window.electronAPI.espPcUtil.connect({
            port: "COM5",
            baudRate: 115200,
          });
        }}
      >
        connect
      </Button>
      <Button
        onPress={async () => {
          await window.electronAPI.espPcUtil.close();
        }}
      >
        disconnect
      </Button>
      <Button
        onPress={async () => {
          await window.electronAPI.espPcUtil.sendJson({ lol: "lol" });
        }}
      >
        lol6
      </Button>
    </div>
  );
}
