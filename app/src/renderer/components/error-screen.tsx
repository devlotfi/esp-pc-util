import { Alert } from "@heroui/react";
import { useTranslation } from "react-i18next";
import ErrorSVG from "./svg/ErrorSVG";

export default function ErrorScreen() {
  const { t } = useTranslation();

  return (
    <div className="flex flex-1 flex-col justify-center items-center">
      <div className="flex flex-col gap-[1rem]">
        <ErrorSVG className="h-[13rem]" />
        <Alert color="danger">{t("error")}</Alert>
      </div>
    </div>
  );
}
