import { useEffect, useState, type PropsWithChildren } from "react";
import { SerialContext } from "../context/serial-context";
import { toast } from "@heroui/react";
import { useTranslation } from "react-i18next";
import { FontAwesomeIcon } from "@fortawesome/react-fontawesome";
import { faInfoCircle } from "@fortawesome/free-solid-svg-icons";
import type { ConnectPayload } from "../../shared/ipc";
import { loadSerialConnection } from "../utils/persist-connection";

export default function SerialProvider({ children }: PropsWithChildren) {
  const { t } = useTranslation();
  const [connected, setConnected] = useState<boolean>(false);
  const [connectionInfo, setConnectionInfo] = useState<ConnectPayload | null>(
    null,
  );

  useEffect(() => {
    if (connected) {
      return;
    }

    const tryConnect = () => {
      console.log("Trying to connect...");

      const serialConnection = loadSerialConnection();

      if (!serialConnection) {
        return;
      }

      window.electronAPI.espPcUtil.connect(serialConnection);
    };

    tryConnect();

    const interval = setInterval(tryConnect, 10000);

    return () => {
      clearInterval(interval);
    };
  }, [connected]);

  useEffect(() => {
    const unsubscribe = window.electronAPI.espPcUtil.onConnected(() => {
      setConnected(true);
      console.log("ESP32 connected");
    });
    return unsubscribe;
  }, []);
  useEffect(() => {
    const unsubscribe = window.electronAPI.espPcUtil.onDisconnected(() => {
      setConnected(false);
      setConnectionInfo(null);
      console.log("ESP32 disconnected");
    });
    return unsubscribe;
  }, []);
  useEffect(() => {
    const unsubscribe = window.electronAPI.espPcUtil.onError((error) => {
      toast(t("error"), {
        indicator: <FontAwesomeIcon icon={faInfoCircle}></FontAwesomeIcon>,
        variant: "danger",
      });
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
    <SerialContext.Provider
      value={{
        connected,
        connectionInfo,
        setConnectionInfo,
      }}
    >
      {children}
    </SerialContext.Provider>
  );
}
