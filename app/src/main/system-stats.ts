import os from "node:os";

export function getRamUsagePercentage(): number {
  const total = os.totalmem();
  const free = os.freemem();

  return Math.round(((total - free) / total) * 100);
}

export function getCpuUsagePercentage(): Promise<number> {
  return new Promise((resolve) => {
    const start = cpuTimes();

    setTimeout(() => {
      const end = cpuTimes();

      const idle = end.idle - start.idle;
      const total = end.total - start.total;

      const usage = 100 - (idle / total) * 100;

      resolve(Math.round(usage));
    }, 100);
  });
}

function cpuTimes() {
  const cpus = os.cpus();

  let idle = 0;
  let total = 0;

  for (const cpu of cpus) {
    idle += cpu.times.idle;

    total +=
      cpu.times.user +
      cpu.times.nice +
      cpu.times.sys +
      cpu.times.irq +
      cpu.times.idle;
  }

  return { idle, total };
}
