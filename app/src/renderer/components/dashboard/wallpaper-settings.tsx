import { useTranslation } from "react-i18next";
import CardWithTitle from "../card-with-header";
import { faImage, faSave } from "@fortawesome/free-solid-svg-icons";
import { Button, cn, Label, Slider } from "@heroui/react";
import { useFormik } from "formik";
import { FontAwesomeIcon } from "@fortawesome/react-fontawesome";
import { useMutation } from "@tanstack/react-query";
import { useRef, useState, type ChangeEvent, type DragEvent } from "react";
import Cropper, { type Area, type Point } from "react-easy-crop";
import {
  getCroppedImg,
  imageToRGB565Base64,
} from "../../utils/react-crop-utils";

export default function WallpaperSettings() {
  const { t } = useTranslation();

  const inputRef = useRef<HTMLInputElement>(null);
  const [dragging, setDragging] = useState(false);
  const [image, setImage] = useState<string | null>(null);
  const [crop, setCrop] = useState<Point>({ x: 0, y: 0 });
  const [zoom, setZoom] = useState(1);
  const [rotation, setRotation] = useState(0);
  const [croppedAreaPixels, setCroppedAreaPixels] = useState<Area | null>(null);
  const [croppedImage, setCroppedImage] = useState<string | null>(null);

  const formik = useFormik({
    enableReinitialize: true,
    initialValues: {
      image: null as string | null,
    },
    async onSubmit(values) {
      if (croppedImage) {
        const base64 = await imageToRGB565Base64(croppedImage);
        console.log({
          type: "SET_WALLPAPER",
          image: base64,
        });
        window.electronAPI.espPcUtil.sendJson({
          type: "SET_WALLPAPER",
          image: base64,
        });
      }
    },
  });

  const createPreviewMutation = useMutation({
    mutationFn: async () => {
      if (!croppedAreaPixels || !image) {
        return;
      }
      setCroppedImage(await getCroppedImg(image, croppedAreaPixels, rotation));
    },
  });

  const saveImageMutation = useMutation({
    mutationFn: async () => {},
  });

  return (
    <CardWithTitle icon={faImage} title={t("wallpaper")}>
      <form
        onSubmit={formik.handleSubmit}
        className="flex flex-col p-[1rem] gap-[1rem]"
      >
        <div
          onClick={() => inputRef.current?.click()}
          onDragOver={(e) => {
            e.preventDefault();
            setDragging(true);
          }}
          onDragLeave={() => setDragging(false)}
          onDrop={(event: DragEvent<HTMLDivElement>) => {
            event.preventDefault();
            setDragging(false);
            const file = event.dataTransfer.files[0];
            if (file) {
              setImage(URL.createObjectURL(file));
            }
          }}
          className={cn(
            "flex flex-col justify-center items-center text-center gap-[0.2rem] py-[1rem] cursor-pointer select-none rounded-2xl",
            dragging
              ? "border-[2px] border-accent border-dashed"
              : "border-[2px] border-separator border-dashed",
          )}
        >
          <input
            ref={inputRef}
            type="file"
            accept="image/*"
            hidden
            onChange={(event: ChangeEvent<HTMLInputElement>) => {
              const file = event.target.files?.[0];
              if (!file) {
                return;
              }
              setImage(URL.createObjectURL(file));
            }}
          />

          <div className="flex">
            <FontAwesomeIcon
              icon={faImage}
              className="text-[3rem]"
            ></FontAwesomeIcon>
          </div>

          <div className="flex flex-col items-center gap-[0.2rem]">
            <div className="flex text-[12pt] font-medium">{t("crop1")}</div>
            <div className="flex text-[10pt]">{t("crop2")}</div>
          </div>
        </div>

        {image ? (
          <>
            <div className="cropper relative min-h-[20rem]">
              <Cropper
                classes={{
                  containerClassName: "rounded-2xl",
                }}
                image={image}
                crop={crop}
                zoom={zoom}
                rotation={rotation}
                maxZoom={5}
                aspect={4 / 3}
                onCropChange={setCrop}
                onZoomChange={setZoom}
                onRotationChange={setRotation}
                onCropComplete={(_: Area, croppedPixels: Area) => {
                  setCroppedAreaPixels(croppedPixels);
                }}
              />
            </div>

            <Slider
              minValue={1}
              maxValue={5}
              step={0.1}
              value={zoom}
              onChange={(value) => {
                if (typeof value === "number") {
                  setZoom(value);
                }
              }}
            >
              <Label>{t("zoom")}</Label>
              <Slider.Output />
              <Slider.Track>
                <Slider.Fill />
                <Slider.Thumb />
              </Slider.Track>
            </Slider>

            <Slider
              minValue={0}
              maxValue={360}
              value={rotation}
              onChange={(value) => {
                if (typeof value === "number") {
                  setRotation(value);
                }
              }}
            >
              <Label>{t("rotation")}</Label>
              <Slider.Output />
              <Slider.Track>
                <Slider.Fill />
                <Slider.Thumb />
              </Slider.Track>
            </Slider>

            <Button
              variant="outline"
              fullWidth
              onPress={async () => createPreviewMutation.mutate()}
              isPending={createPreviewMutation.isPending}
            >
              {t("preview")} <FontAwesomeIcon icon={faImage}></FontAwesomeIcon>
            </Button>

            {croppedImage ? (
              <div className="flex flex-col items-center border border-border rounded-2xl">
                <img
                  src={croppedImage}
                  alt="Cropped result"
                  className="h-[240px] w-[320px]"
                />
              </div>
            ) : null}
          </>
        ) : null}

        <Button fullWidth type="submit" isPending={saveImageMutation.isPending}>
          {t("save")} <FontAwesomeIcon icon={faSave}></FontAwesomeIcon>
        </Button>
      </form>
    </CardWithTitle>
  );
}
