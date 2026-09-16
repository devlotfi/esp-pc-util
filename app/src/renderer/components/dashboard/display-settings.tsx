import { Label, ListBox, Select } from "@heroui/react";
import { useContext } from "react";
import { useTranslation } from "react-i18next";
import { ThemeContext } from "../../context/theme-context";
import { ThemeOptions } from "../../types/theme-options";
import { renderFlag } from "../../utils/render-flag";
import CardWithTitle from "../card-with-header";
import {
  faComputer,
  faMoon,
  faPaintBrush,
  faSun,
} from "@fortawesome/free-solid-svg-icons";
import { FontAwesomeIcon } from "@fortawesome/react-fontawesome";

export default function DisplaySettings() {
  const { themeOption, setTheme } = useContext(ThemeContext);
  const { t, i18n } = useTranslation();

  return (
    <CardWithTitle icon={faPaintBrush} title={t("display")}>
      <div className="flex flex-col p-[1rem] gap-[1rem]">
        <Select
          value={themeOption}
          onChange={(value) => setTheme(value?.toString() as ThemeOptions)}
        >
          <Label>{t("theme")}</Label>
          <Select.Trigger>
            <Select.Value />
            <Select.Indicator />
          </Select.Trigger>
          <Select.Popover>
            <ListBox>
              <ListBox.Item
                key={ThemeOptions.SYSTEM}
                id={ThemeOptions.SYSTEM}
                textValue={t("system")}
              >
                <div className="flex gap-[1rem] items-center">
                  <div className="flex justify-center items-center h-[2rem] w-[2rem] bg-accent text-accent-foreground rounded-2xl">
                    <FontAwesomeIcon icon={faComputer}></FontAwesomeIcon>
                  </div>
                  <div className="flex">{t("system")}</div>
                </div>
                <ListBox.ItemIndicator />
              </ListBox.Item>
              <ListBox.Item
                key={ThemeOptions.LIGHT}
                id={ThemeOptions.LIGHT}
                textValue={t("light")}
              >
                <div className="flex gap-[1rem] items-center">
                  <div className="flex justify-center items-center h-[2rem] w-[2rem] bg-accent text-accent-foreground rounded-2xl">
                    <FontAwesomeIcon icon={faSun}></FontAwesomeIcon>
                  </div>
                  <div className="flex">{t("light")}</div>
                </div>
                <ListBox.ItemIndicator />
              </ListBox.Item>
              <ListBox.Item
                key={ThemeOptions.DARK}
                id={ThemeOptions.DARK}
                textValue={t("dark")}
              >
                <div className="flex gap-[1rem] items-center">
                  <div className="flex justify-center items-center h-[2rem] w-[2rem] bg-accent text-accent-foreground rounded-2xl">
                    <FontAwesomeIcon icon={faMoon}></FontAwesomeIcon>
                  </div>
                  <div className="flex">{t("dark")}</div>
                </div>
                <ListBox.ItemIndicator />
              </ListBox.Item>
            </ListBox>
          </Select.Popover>
        </Select>

        <Select
          value={i18n.language}
          onChange={(value) => i18n.changeLanguage(value?.toString())}
        >
          <Label>{t("language")}</Label>
          <Select.Trigger>
            <Select.Value />
            <Select.Indicator />
          </Select.Trigger>
          <Select.Popover>
            <ListBox>
              <ListBox.Item key={"ar"} id={"ar"} textValue={"العربية"}>
                <div className="flex gap-[1rem] items-center">
                  <div className="flex justify-center items-center h-[2rem] w-[2rem] rounded-lg">
                    {renderFlag("ar")}
                  </div>
                  <div className="flex">العربية</div>
                </div>
                <ListBox.ItemIndicator />
              </ListBox.Item>
              <ListBox.Item key={"fr"} id={"fr"} textValue={"Français"}>
                <div className="flex gap-[1rem] items-center">
                  <div className="flex justify-center items-center h-[2rem] w-[2rem] rounded-lg">
                    {renderFlag("fr")}
                  </div>
                  <div className="flex">Français</div>
                </div>
                <ListBox.ItemIndicator />
              </ListBox.Item>
              <ListBox.Item key={"en"} id={"en"} textValue={"English"}>
                <div className="flex gap-[1rem] items-center">
                  <div className="flex justify-center items-center h-[2rem] w-[2rem] rounded-lg">
                    {renderFlag("en")}
                  </div>
                  <div className="flex">English</div>
                </div>
                <ListBox.ItemIndicator />
              </ListBox.Item>
            </ListBox>
          </Select.Popover>
        </Select>
      </div>
    </CardWithTitle>
  );
}
