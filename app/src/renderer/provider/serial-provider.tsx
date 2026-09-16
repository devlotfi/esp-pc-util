import { useEffect, useState, type PropsWithChildren } from "react";
import { SerialContext } from "../context/serial-context";
import { toast } from "@heroui/react";
import { useTranslation } from "react-i18next";
import { FontAwesomeIcon } from "@fortawesome/react-fontawesome";
import { faInfoCircle } from "@fortawesome/free-solid-svg-icons";

export default function SerialProvider({ children }: PropsWithChildren) {
  const { t } = useTranslation();
  const [connected, setConnected] = useState<boolean>(false);

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
      }}
    >
      {children}
    </SerialContext.Provider>
  );
}
