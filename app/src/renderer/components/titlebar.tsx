import { Button } from "@heroui/react";
import { FontAwesomeIcon } from "@fortawesome/react-fontawesome";
import {
  faTimes,
  faWindowMaximize,
  faWindowMinimize,
} from "@fortawesome/free-solid-svg-icons";
import LogoSVG from "./svg/LogoSVG";

export default function Titlebar() {
  return (
    <div className="titlebar flex justify-between items-center px-[0.8rem] h-[3.5rem]">
      <div className="flex items-center gap-[1rem]">
        <LogoSVG className="h-[2rem]"></LogoSVG>
        <div className="flex font-medium">ESP PC Util</div>
      </div>

      <div className="titlebar-ui flex items-center gap-[0.5rem]">
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
          onPress={async () => await window.electronAPI.window.close()}
        >
          <FontAwesomeIcon icon={faTimes}></FontAwesomeIcon>
        </Button>
      </div>
    </div>
  );
}
