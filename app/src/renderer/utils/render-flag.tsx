import ArSVG from "../assets/flags/ar.svg";
import FrSVG from "../assets/flags/fr.svg";
import EnSVG from "../assets/flags/en.svg";
import { cn } from "@heroui/styles";

export const renderFlag = (languageCode: string, className?: string) => {
  switch (languageCode) {
    case "ar":
      return (
        <img src={ArSVG} alt="ar" className={cn("h-[1.5rem]", className)}></img>
      );
    case "fr":
      return (
        <img src={FrSVG} alt="fr" className={cn("h-[1.5rem]", className)}></img>
      );
    case "en":
      return (
        <img src={EnSVG} alt="eb" className={cn("h-[1.5rem]", className)}></img>
      );
  }
};
