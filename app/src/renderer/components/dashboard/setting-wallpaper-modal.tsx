import { Modal, Spinner, type UseOverlayStateReturn } from "@heroui/react";
import { useEffect } from "react";
import { useTranslation } from "react-i18next";

interface SettingWallpaperModalProps {
  state: UseOverlayStateReturn;
}

export default function SettingWallpaperModal({
  state,
}: SettingWallpaperModalProps) {
  const { t } = useTranslation();

  useEffect(() => {
    const unsubscribe = window.electronAPI.espPcUtil.onJson((json) => {
      console.log("json", json);
      if (json.type === "SET_WALLPAPER_COMPLETED") {
        state.close();
      }
    });
    return unsubscribe;
  }, []);

  return (
    <Modal.Backdrop isOpen={state.isOpen} onOpenChange={state.setOpen}>
      <Modal.Container>
        <Modal.Dialog>
          <Modal.Body className="flex flex-col justify-center items-center gap-[0.5rem] p-[1rem]">
            <Spinner size="lg"></Spinner>
            <div className="flex font-medium text-[15pt]">
              {t("settingWallpaper")}
            </div>
          </Modal.Body>
        </Modal.Dialog>
      </Modal.Container>
    </Modal.Backdrop>
  );
}
