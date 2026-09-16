import { useTranslation } from "react-i18next";
import CardWithTitle from "../card-with-header";
import { faLightbulb, faSave } from "@fortawesome/free-solid-svg-icons";
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

export default function LedSettings() {
  const { t } = useTranslation();

  const formik = useFormik({
    enableReinitialize: true,
    initialValues: {
      color: parseColor("#000000"),
      brightness: 0,
    },
    onSubmit(values) {
      mutate({
        color: values.color.toString("hex"),
        brightness: values.brightness,
      });
    },
  });

  const { mutate, isPending } = useMutation({
    mutationFn: async (
      payload: Omit<Extract<JsonMessage, { type: "SET_LED" }>, "type">,
    ) => {
      await window.electronAPI.espPcUtil.sendJson({
        type: "SET_LED",
        ...payload,
      });
    },
  });

  return (
    <CardWithTitle icon={faLightbulb} title={t("rgbLed")}>
      <form
        onSubmit={formik.handleSubmit}
        className="flex flex-col p-[1rem] gap-[1rem]"
      >
        <ColorField
          aria-label="color"
          value={formik.values.color}
          onChange={(value) => formik.setFieldValue("color", value)}
        >
          <ColorField.Group>
            <ColorField.Prefix>
              <ColorPicker
                value={formik.values.color}
                onChange={(value) => formik.setFieldValue("color", value)}
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
          maxValue={255}
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
