import type { IconProp } from "@fortawesome/fontawesome-svg-core";
import { FontAwesomeIcon } from "@fortawesome/react-fontawesome";
import { cn } from "@heroui/react";
import { type ComponentProps, type PropsWithChildren } from "react";

interface SectionHeaderProps extends ComponentProps<"div"> {
  icon: IconProp;
  iconProps?: {
    className: string;
  };
  iconWrapperProps?: ComponentProps<"div">;
  labelProps?: ComponentProps<"div">;
}

export default function SectionHeader({
  children,
  icon,
  iconProps,
  className,
  iconWrapperProps: {
    className: classNameIconWrapper,
    ...iconWrapperProps
  } = {},
  labelProps: { className: classNameLabel, ...labelProps } = {},
  ...props
}: PropsWithChildren<SectionHeaderProps>) {
  return (
    <div
      className={cn(
        "flex items-center gap-[1rem] md:gap-[2rem] py-[2rem]",
        className,
      )}
      {...props}
    >
      <div
        className={cn(
          "flex justify-center items-center rounded-2xl size-[3rem] bg-accent",
          classNameIconWrapper,
        )}
        {...iconWrapperProps}
      >
        <FontAwesomeIcon
          icon={icon}
          className={cn(
            "text-accent-foreground size-[2rem]",
            iconProps?.className,
          )}
        ></FontAwesomeIcon>
      </div>
      <div
        className={cn("flex font-bold text-[20pt]", classNameLabel)}
        {...labelProps}
      >
        {children}
      </div>
    </div>
  );
}
