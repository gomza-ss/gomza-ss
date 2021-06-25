В папках этого уровня лежат taskConfig.json и demoConfig.json для каждой задачи.

Для параллельной обработки кадров используйте параметр ```nireq``` в taskConfig. В задачах с устройствами NVIDIA в поле ```device``` указывайте GUID целевой видеокарты.

Например так выглядит обработка кадров в Caffe2Auto с двумя исполнителями на видеокарте ```GPU-3af28c99-7f1d-da06-da33-45d075f75d04```:

```
{
    "module": "Auto",
    "task": "LPDet",
    "deviceVendor": "NVIDIA",
    "input_image_width": 480,
    "input_image_height": 270,
    "device": "GPU-3af28c99-7f1d-da06-da33-45d075f75d04"
    "nireq": 2
}
```
