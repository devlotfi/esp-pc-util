import { Button, Label, ListBox, Select, Tabs } from "@heroui/react";
import { useMutation, useQuery } from "@tanstack/react-query";
import { createFileRoute, Navigate } from "@tanstack/react-router";
import LoadingScreen from "../components/loading-screen";
import ErrorScreen from "../components/error-screen";
import { faPlug } from "@fortawesome/free-solid-svg-icons";
import CardWithTitle from "../components/card-with-header";
import { useTranslation } from "react-i18next";
import { FontAwesomeIcon } from "@fortawesome/react-fontawesome";
import { useFormik } from "formik";
import type { BaudRate, ConnectPayload } from "../../shared/ipc";
import { useContext } from "react";
import { SerialContext } from "../context/serial-context";
import * as yup from "yup";
import AppSettings from "../components/dashboard/app-settings";
import { saveSerialConnection } from "../utils/persist-connection";

export const BAUD_RATES = [
  110, 300, 600, 1200, 2400, 4800, 9600, 14400, 19200, 28800, 38400, 56000,
  57600, 115200, 128000, 230400, 256000, 460800, 500000, 576000, 921600,
  1000000, 1152000, 1500000, 2000000, 2500000, 3000000, 4000000,
] as const;

export const Route = createFileRoute("/")({
  component: RouteComponent,
});

function RouteComponent() {
  const { t } = useTranslation();
  const { connected, setConnectionInfo } = useContext(SerialContext);

  const formik = useFormik({
    initialValues: {
      port: "",
      baudRate: 115200 as BaudRate,
    },
    validationSchema: yup.object({
      port: yup.string().required(),
      baudRate: yup.number().integer().required(),
    }),
    onSubmit(values) {
      mutate(values);
    },
  });

  const { data, isLoading, isError } = useQuery({
    queryFn: async () => {
      const ports = await window.electronAPI.espPcUtil.listPorts();
      return ports;
    },
    queryKey: ["PORTS"],
  });

  const { mutate, isPending } = useMutation({
    mutationFn: async (payload: ConnectPayload) => {
      await window.electronAPI.espPcUtil.connect(payload);
      setConnectionInfo(payload);
      saveSerialConnection(payload);
    },
  });

  if (connected) return <Navigate to="/dashboard"></Navigate>;
  if (isLoading) return <LoadingScreen></LoadingScreen>;
  if (isError || !data) return <ErrorScreen></ErrorScreen>;

  return (
    <div className="flex flex-1 justify-center items-center">
      <Tabs className="w-full max-w-md">
        <Tabs.ListContainer>
          <Tabs.List aria-label="Options">
            <Tabs.Tab id="CONNECT">
              {t("connect")}
              <Tabs.Indicator />
            </Tabs.Tab>
            <Tabs.Tab id="APP_SETTINGS">
              {t("appSettings")}
              <Tabs.Indicator />
            </Tabs.Tab>
          </Tabs.List>
        </Tabs.ListContainer>
        <Tabs.Panel className="pt-3" id="CONNECT">
          <CardWithTitle
            icon={faPlug}
            title={t("connect")}
            className="min-w-[25rem]"
          >
            <form
              onSubmit={formik.handleSubmit}
              className="flex flex-1 flex-col p-[1rem] gap-[1rem]"
            >
              <Select
                value={formik.values.port}
                onChange={(value) => {
                  if (value) {
                    formik.setFieldValue("port", value.toString());
                  }
                }}
              >
                <Label>{t("port")}</Label>
                <Select.Trigger>
                  <Select.Value />
                  <Select.Indicator />
                </Select.Trigger>
                <Select.Popover>
                  <ListBox>
                    {data.map((port) => (
                      <ListBox.Item
                        key={port.path}
                        id={port.path}
                        textValue={port.path}
                      >
                        {port.path}
                        <ListBox.ItemIndicator />
                      </ListBox.Item>
                    ))}
                  </ListBox>
                </Select.Popover>
              </Select>

              <Select
                value={formik.values.baudRate}
                onChange={(value) => {
                  if (value) {
                    formik.setFieldValue("baudRate", value.toString());
                  }
                }}
              >
                <Label>{t("baudRate")}</Label>
                <Select.Trigger>
                  <Select.Value />
                  <Select.Indicator />
                </Select.Trigger>
                <Select.Popover>
                  <ListBox>
                    {BAUD_RATES.map((baudRate) => (
                      <ListBox.Item
                        key={baudRate}
                        id={baudRate}
                        textValue={`${baudRate}`}
                      >
                        {baudRate}
                        <ListBox.ItemIndicator />
                      </ListBox.Item>
                    ))}
                  </ListBox>
                </Select.Popover>
              </Select>

              <Button fullWidth type="submit" isPending={isPending}>
                {t("connect")} <FontAwesomeIcon icon={faPlug}></FontAwesomeIcon>
              </Button>
            </form>
          </CardWithTitle>
        </Tabs.Panel>
        <Tabs.Panel className="pt-3" id="APP_SETTINGS">
          <AppSettings></AppSettings>
        </Tabs.Panel>
      </Tabs>
    </div>
  );
}
