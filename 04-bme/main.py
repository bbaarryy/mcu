import time
import serial
import matplotlib.pyplot as plt


def read_value(ser):
    """Читает строку из порта и возвращает float. Пропускает нераспознанные строки."""
    while True:
        try:
            line = ser.readline().decode('ascii').strip()
            if not line:
                continue
            # Парсим формат "temperature: 23.15 C" / "pressure: 1013.25 hPa" / "humidity: 50.0 %"
            parts = line.split()
            return float(parts[1])
        except (ValueError, IndexError):
            continue


def main():
    ser = serial.Serial(port='/dev/ttyACM0', baudrate=115200, timeout=1.0)
    if ser.is_open:
        print(f"Port {ser.name} opened")
    else:
        print(f"Port {ser.name} closed")

    measure_temp_C   = []
    measure_pres_hPa = []
    measure_hum_pct  = []
    measure_ts       = []
    start_ts = time.time()

    try:
        while True:
            ts = time.time() - start_ts

            ser.write("temp\n".encode('ascii'))
            temp = read_value(ser)

            ser.write("pres\n".encode('ascii'))
            pres = read_value(ser)

            ser.write("hum\n".encode('ascii'))
            hum = read_value(ser)

            measure_ts.append(ts)
            measure_temp_C.append(temp)
            measure_pres_hPa.append(pres)
            measure_hum_pct.append(hum)

            print(f'{ts:.1f}s | {temp:.2f} C | {pres:.2f} hPa | {hum:.1f} %')

            time.sleep(0.5)

    finally:
        ser.close()
        print("Port closed")

        fig, axes = plt.subplots(3, 1, figsize=(10, 8))

        axes[0].plot(measure_ts, measure_temp_C)
        axes[0].set_title('Температура')
        axes[0].set_xlabel('время, с')
        axes[0].set_ylabel('температура, °C')

        axes[1].plot(measure_ts, measure_pres_hPa)
        axes[1].set_title('Давление')
        axes[1].set_xlabel('время, с')
        axes[1].set_ylabel('давление, Па')

        axes[2].plot(measure_ts, measure_hum_pct)
        axes[2].set_title('Влажность')
        axes[2].set_xlabel('время, с')
        axes[2].set_ylabel('влажность, %')

        plt.tight_layout()
        plt.show()


if __name__ == "__main__":
    main()