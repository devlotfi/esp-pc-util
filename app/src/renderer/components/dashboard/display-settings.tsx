import { useTranslation } from "react-i18next";
import CardWithTitle from "../card-with-header";
import { faSave, faTv } from "@fortawesome/free-solid-svg-icons";
import {
  Button,
  ColorArea,
  ColorField,
  ColorPicker,
  ColorSlider,
  ColorSwatch,
  Label,
  parseColor,
  Slider,
} from "@heroui/react";
import { useFormik } from "formik";
import { FontAwesomeIcon } from "@fortawesome/react-fontawesome";
import { useMutation } from "@tanstack/react-query";
import type { JsonMessage } from "../../../shared/types/json-message";
import { useEffect } from "react";

export default function DisplaySettings() {
  const { t } = useTranslation();

  const formik = useFormik({
    enableReinitialize: true,
    initialValues: {
      accentColor: parseColor("#000000"),
      brightness: 0,
    },
    onSubmit(values) {
      mutate({
        accentColor: values.accentColor.toString("hex"),
        brightness: values.brightness,
      });
    },
  });

  const { mutate, isPending } = useMutation({
    mutationFn: async (
      payload: Omit<Extract<JsonMessage, { type: "SET_DISPLAY" }>, "type">,
    ) => {
      await window.electronAPI.espPcUtil.sendJson({
        type: "SET_DISPLAY",
        ...payload,
      });
    },
  });

  useEffect(() => {
    window.electronAPI.espPcUtil.sendJson({
      type: "GET_DATA",
    });
  }, []);

  useEffect(() => {
    const unsubscribe = window.electronAPI.espPcUtil.onJson((json) => {
      console.log("json", json);
      if (json.type === "GET_DATA_RESPONSE") {
        formik.setFieldValue(
          "accentColor",
          parseColor(json.display.accentColor),
        );
        formik.setFieldValue("brightness", json.display.brightness);
      }
    });
    return unsubscribe;
  }, []);

  return (
    <CardWithTitle icon={faTv} title={t("display")}>
      <form
        onSubmit={formik.handleSubmit}
        className="flex flex-col p-[1rem] gap-[1rem]"
      >
        <ColorField
          aria-label="accentColor"
          value={formik.values.accentColor}
          onChange={(value) => formik.setFieldValue("accentColor", value)}
        >
          <Label>{t("accentColor")}</Label>
          <ColorField.Group>
            <ColorField.Prefix>
              <ColorPicker
                value={formik.values.accentColor}
                onChange={(value) => formik.setFieldValue("accentColor", value)}
              >
                <ColorPicker.Trigger>
                  <ColorSwatch size="sm" />
                </ColorPicker.Trigger>
                <ColorPicker.Popover className="gap-2">
                  <ColorArea
                    aria-label="Color area"
                    className="max-w-full"
                    colorSpace="hsb"
                    xChannel="saturation"
                    yChannel="brightness"
                  >
                    <ColorArea.Thumb />
                  </ColorArea>

                  <ColorSlider
                    aria-label="Hue slider"
                    channel="hue"
                    className="flex-1"
                    colorSpace="hsb"
                  >
                    <ColorSlider.Track>
                      <ColorSlider.Thumb />
                    </ColorSlider.Track>
                  </ColorSlider>
                </ColorPicker.Popover>
              </ColorPicker>
            </ColorField.Prefix>
            <ColorField.Input />
          </ColorField.Group>
        </ColorField>

        <Slider
          minValue={0}
          maxValue={100}
          value={formik.values.brightness}
          onChange={(value) => {
            if (typeof value === "number") {
              formik.setFieldValue("brightness", value);
            }
          }}
        >
          <Label>{t("brightness")}</Label>
          <Slider.Output />
          <Slider.Track>
            <Slider.Fill />
            <Slider.Thumb />
          </Slider.Track>
        </Slider>

        <Button fullWidth type="submit" isPending={isPending}>
          {t("save")} <FontAwesomeIcon icon={faSave}></FontAwesomeIcon>
        </Button>
      </form>
    </CardWithTitle>
  );
}
