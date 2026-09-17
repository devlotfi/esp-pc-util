import { Button, Chip } from "@heroui/react";
import { FontAwesomeIcon } from "@fortawesome/react-fontawesome";
import {
  faPlugCircleCheck,
  faPlugCircleXmark,
  faTimes,
  faWindowMaximize,
  faWindowMinimize,
} from "@fortawesome/free-solid-svg-icons";
import LogoSVG from "./svg/LogoSVG";
import { useContext } from "react";
import { SerialContext } from "../context/serial-context";
import { useTranslation } from "react-i18next";
import { useMutation } from "@tanstack/react-query";
import { clearSerialConnection } from "../utils/persist-connection";

export default function Titlebar() {
  const { t } = useTranslation();
  const { connected, connectionInfo } = useContext(SerialContext);

  const { mutate, isPending } = useMutation({
    mutationFn: async () => {
      clearSerialConnection();
      await window.electronAPI.espPcUtil.close();
    },
  });

  return (
    <div className="titlebar flex relative justify-center items-center h-[3.5rem]">
      <div className="flex left-[0.8rem] absolute items-center gap-[1rem]">
        <LogoSVG className="h-[2rem]"></LogoSVG>
        <div className="flex font-medium">ESP PC Util</div>
      </div>

      <div className="flex items-center gap-[0.5rem]">
        {connected ? (
          <Chip size="lg" variant="primary" color="success">
            <FontAwesomeIcon icon={faPlugCircleCheck}></FontAwesomeIcon>
            <Chip.Label>{t("connected")}</Chip.Label>
            {connectionInfo ? (
              <Chip size="sm" variant="secondary" color="success">
                <Chip.Label>
                  {connectionInfo.port} / {connectionInfo.baudRate}
                </Chip.Label>
              </Chip>
            ) : null}
          </Chip>
        ) : (
          <Chip size="lg" variant="primary" color="danger">
            <FontAwesomeIcon icon={faPlugCircleXmark}></FontAwesomeIcon>
            <Chip.Label>{t("disconnected")}</Chip.Label>
          </Chip>
        )}

        {connected ? (
          <Button
            isIconOnly
            size="sm"
            variant="outline"
            className="titlebar-ui"
            onPress={() => mutate()}
          >
            <FontAwesomeIcon icon={faTimes}></FontAwesomeIcon>
          </Button>
        ) : null}
      </div>

      <div className="titlebar-ui flex right-[0.8rem] absolute items-center gap-[0.5rem]">
        <Button
          isIconOnly
          variant="outline"
          onPress={() => window.electronAPI.window.minimize()}
        >
          <FontAwesomeIcon icon={faWindowMinimize}></FontAwesomeIcon>
        </Button>
        <Button
          isIconOnly
          variant="outline"
          onPress={() => window.electronAPI.window.maximize()}
        >
          <FontAwesomeIcon icon={faWindowMaximize}></FontAwesomeIcon>
        </Button>

        <Button
          isIconOnly
          variant="outline"
          onPress={() => window.electronAPI.window.close()}
          isPending={isPending}
        >
          <FontAwesomeIcon icon={faTimes}></FontAwesomeIcon>
        </Button>
      </div>
    </div>
  );
}
