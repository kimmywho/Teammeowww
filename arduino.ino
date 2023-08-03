void setup()
{
    // Initialize the Serial communication at a baud rate of 115200
    Serial.begin(115200);

    // Wait until a Serial connection is established (only required for native USB)
    while (!Serial);

#ifdef VERBOSE_OUTPUT // Check if VERBOSE_OUTPUT is defined to display additional information
    // Print a welcome message for the Edge Impulse Inferencing Demo
    Serial.println("Edge Impulse Inferencing Demo");

    // Display a summary of the inferencing settings (from model_metadata.h)
    ei_printf("Inferencing settings:\n");
    // Print the image resolution used by the classifier
    ei_printf("\tImage resolution: %dx%d\n", EI_CLASSIFIER_INPUT_WIDTH, EI_CLASSIFIER_INPUT_HEIGHT);
    // Print the size of a frame (input data) used for inference on the DSP (Digital Signal Processor)
    ei_printf("\tFrame size: %d\n", EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE);
    // Print the number of classes the classifier can detect
    ei_printf("\tNo. of classes: %d\n", sizeof(ei_classifier_inferencing_categories) / sizeof(ei_classifier_inferencing_categories[0]));
#endif
}
void loop()
{
    // ... Other Codes ...

    // Print the predictions
#ifdef VERBOSE_OUTPUT
    // Print the time taken for DSP (Digital Signal Processor) processing, classification, and anomaly detection
    ei_printf("Predictions (DSP: %d ms., Classification: %d ms., Anomaly: %d ms.): \n",
              result.timing.dsp, result.timing.classification, result.timing.anomaly);
#endif

    // Object Detection
#if EI_CLASSIFIER_OBJECT_DETECTION == 1
    // Check if any bounding box with positive value (object detected) is found
    bool bb_found = result.bounding_boxes[0].value > 0;

    // Loop through all bounding boxes and display their details if they contain an object
    for (size_t ix = 0; ix < result.bounding_boxes_count; ix++) {
        auto bb = result.bounding_boxes[ix];
        if (bb.value == 0) {
            continue; // Skip if no object is detected in the current bounding box
        }

        // Print the label, value (confidence score), and the position and size of the bounding box
        ei_printf("    %s (%f) [ x: %u, y: %u, width: %u, height: %u ]\n", bb.label, bb.value, bb.x, bb.y, bb.width, bb.height);
    }

    // Print a message if no objects are found in the current frame
    if (!bb_found) {
        ei_printf("    No objects found\n");
    }
#else
#ifdef VERBOSE_OUTPUT
    // Classification (if not object detection)
    // Loop through all classification labels and print the label and its corresponding value (probability)
    for (size_t ix = 0; ix < EI_CLASSIFIER_LABEL_COUNT; ix++) {
        ei_printf("    %s: %.5f\n", result.classification[ix].label,
                                    result.classification[ix].value);
    }
#endif

    // Send the possibility of a detected fire (assumes the first class is "fire")
    Serial.println(result.classification[0].value);

    // Anomaly detection score (if supported by the model)
#if EI_CLASSIFIER_HAS_ANOMALY == 1
    ei_printf("    anomaly score: %.3f\n", result.anomaly);
#endif
#endif

    // Check if the user sent 'b' character over serial to stop inferencing
    while (ei_get_serial_available() > 0) {
        if (ei_get_serial_byte() == 'b') {
            ei_printf("Inferencing stopped by user\r\n");
            stop_inferencing = true; // Set a flag to stop inferencing
        }
    }

    // Free snapshot memory if it was allocated during inference
    if (snapshot_mem) ei_free(snapshot_mem);

    // Deinitialize the camera (assuming it's an external camera module)
    ei_camera_deinit();
}
