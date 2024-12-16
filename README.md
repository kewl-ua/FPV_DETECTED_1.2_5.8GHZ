# FPV DETECTED 1.2GHZ 5.8GHZ
> Цей прилад дозволяє виявити наближення FPV, дистанція залежи від місцевості при потужності 200mv та 1,8 метра від землі в полі 1.6 км шо дозволе заїбца. 

## Необхідні компоненти
Прилад складається з:
1.	[Raspberry Pi Pico](https://www.aliexpress.com/item/1005005714757487.html)
2.	[RX5808](https://www.aliexpress.com/item/1005006706936487.html)
3.	[TA8804](https://www.olx.ua/d/uk/obyavlenie/vdeopriymach-tv-tyuner-ta8804f-1-2-1-3-1-5ggts-vrx-920-1680mgts-IDWtqF1.html)
4.	[MT3608](https://www.aliexpress.com/item/1005006334953525.html?spm=a2g0o.order_list.order_list_main.88.eac11802Qjr3ol)

> [!IMPORTANT]
> Потребление схемы до 1.5W (250mAh / 5V)

При виявленні помилок, або пропозицій прошу писати в [особисті](https://t.me/ed_ryb1).
> [!CAUTION]
> Увага Пристрої я не збираю навіть як що дуже треба!!!

## ШО КУДИ тикати
[ПДФ](https://github.com/edwardrybka/FPV_DETECTED_1.2_5.8GHZ/blob/main/FPV_DETECTED.pdf) с полной распиновкой.

Приклад:
![Приклад](/photo_1.jpg)


## **Збірка через Platform IO**

### ***Встанлюємо залежності***
``` pio lib install ```

### **Збираємо прошивку**
``` pio run ```

### ***Підключаємо R Pi Pico та завантажуємо прошивку***
``` pio run --target upload ```
