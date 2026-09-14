import { Button, Separator } from "@heroui/react";
import { useNavigate } from "@tanstack/react-router";
import { useTranslation } from "react-i18next";
import Error404SVG from "./svg/Error404SVG";

export default function NotFound() {
  const { t } = useTranslation();
  const navigate = useNavigate();

  return (
    <div className="flex flex-1 flex-col justify-center items-center px-[1rem]">
      <div className="flex flex-col items-center gap-[1rem]">
        <Error404SVG className="h-[10rem]" />
        <div className="flex text-[13pt] font-medium text-center">
          {t("notFound")}
        </div>
        <Separator></Separator>

        <Button
          fullWidth
          onPress={() =>
            navigate({
              to: "/",
            })
          }
        >
          {t("login")}
        </Button>
      </div>
    </div>
  );
}
