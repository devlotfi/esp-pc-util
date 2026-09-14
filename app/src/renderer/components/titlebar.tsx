import { Button } from "@heroui/react";
import { FontAwesomeIcon } from "@fortawesome/react-fontawesome";
import {
  faTimes,
  faWindowMaximize,
  faWindowMinimize,
} from "@fortawesome/free-solid-svg-icons";

export default function Titlebar() {
  return (
    <div className="titlebar bg-surface flex justify-between items-center px-[1rem] h-[4rem]">
      <h1>lol</h1>

      <div className="titlebar-ui flex items-center gap-[0.5rem]">
        <Button isIconOnly onPress={() => window.electronAPI.window.minimize()}>
          <FontAwesomeIcon icon={faWindowMinimize}></FontAwesomeIcon>
        </Button>
        <Button isIconOnly onPress={() => window.electronAPI.window.maximize()}>
          <FontAwesomeIcon icon={faWindowMaximize}></FontAwesomeIcon>
        </Button>
        <Button
          isIconOnly
          onPress={async () => await window.electronAPI.window.close()}
        >
          <FontAwesomeIcon icon={faTimes}></FontAwesomeIcon>
        </Button>
      </div>
    </div>
  );
}
