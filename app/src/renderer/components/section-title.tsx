import { cn } from "@heroui/react";
import { type ComponentProps } from "react";
import type { IconProp } from "@fortawesome/fontawesome-svg-core";
import { FontAwesomeIcon } from "@fortawesome/react-fontawesome";

export function SectionTitle({
  children,
  icon,
  className,
  iconWrapperProps: {
    className: classNameIconWrapper,
    ...iconWrapperProps
  } = {},
  labelProps: { className: classNameLabel, ...labelProps } = {},
  ...props
}: {
  icon: IconProp;
  iconWrapperProps?: ComponentProps<"div">;
  labelProps?: ComponentProps<"div">;
} & ComponentProps<"div">) {
  return (
    <div
      className={cn("flex items-center gap-[1rem] pb-[1rem]", className)}
      {...props}
    >
      <div
        className={cn(
          "flex justify-center items-center rounded-2xl size-[2.5rem] border",
          classNameIconWrapper,
        )}
        {...iconWrapperProps}
      >
        <FontAwesomeIcon
          icon={icon}
          className="text-accent text-[15pt]"
        ></FontAwesomeIcon>
      </div>
      <div
        className={cn("flex font-bold text-[16pt]", classNameLabel)}
        {...labelProps}
      >
        {children}
      </div>
    </div>
  );
}
