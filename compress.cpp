#include <iostream>
#include <opencv2/opencv.hpp>
#include <thread>
#include <iomanip>

using namespace std;
using namespace cv;

void compressImage(Mat& image, int compression_level)
{
    vector<uchar> buf;
    vector<int> params;
    params.push_back(IMWRITE_JPEG_QUALITY);
    params.push_back(compression_level);

    imencode(".jpg", image, buf, params);
    image = imdecode(buf, IMREAD_COLOR);
}

int main()
{

    Mat image = imread("path-to-your-file(img)", IMREAD_COLOR);
    if (image.empty())
    {
        cout << "Failed to read image." << endl;
        return -1;
    }

   
    cout << "Enter the compression level (0-100): ";
    int compression_level;
    cin >> compression_level;

   
    Mat compressed_image = image.clone();

    // Perform image compression using multithreading
    thread compression_thread(compressImage, ref(compressed_image), compression_level);
    compression_thread.join();

   
    vector<uchar> original_buf, compressed_buf;
    imencode(".jpg", image, original_buf);
    imencode(".jpg", compressed_image, compressed_buf);
    size_t original_size = original_buf.size();
    size_t compressed_size = compressed_buf.size();

   
    double compression_percentage = 100.0 - (compressed_size / (double)original_size * 100.0);

    // Convert the compression percentage to a string
    stringstream ss;
    ss << fixed << setprecision(2) << compression_percentage << "%";
    string compression_text = ss.str();

    // Convert the sizes to strings
    stringstream ss_orig_size, ss_comp_size;
    ss_orig_size << fixed << setprecision(2) << "Original size: " << original_size / 1024.0 << " KB";
    ss_comp_size << fixed << setprecision(2) << "Compressed size: " << compressed_size / 1024.0 << " KB";
    string original_size_text = ss_orig_size.str();
    string compressed_size_text = ss_comp_size.str();

    // Create a combined image to display both original and compressed images side by side
    Mat combined_image(max(image.rows, compressed_image.rows), image.cols + compressed_image.cols, image.type());
    Mat left(combined_image, Rect(0, 0, image.cols, image.rows));
    image.copyTo(left);
    Mat right(combined_image, Rect(image.cols, 0, compressed_image.cols, compressed_image.rows));
    compressed_image.copyTo(right);

    // Annotate the combined image
    putText(combined_image, "Original Image", Point(10, 30), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2, LINE_AA);
    putText(combined_image, original_size_text, Point(10, 70), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2, LINE_AA);
    putText(combined_image, "Compressed Image", Point(image.cols + 10, 30), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2, LINE_AA);
    putText(combined_image, compressed_size_text, Point(image.cols + 10, 70), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2, LINE_AA);
    putText(combined_image, "Compression: " + compression_text, Point(image.cols + 10, 110), FONT_HERSHEY_SIMPLEX, 1, Scalar(255, 255, 255), 2, LINE_AA);

    // Display the combined image
    namedWindow("Comparison", WINDOW_NORMAL);
    imshow("Comparison", combined_image);
    waitKey(0);


    cout << "Original image size: " << original_size << " bytes" << endl;
    cout << "Compressed image size: " << compressed_size << " bytes" << endl;
    cout << "Compression percentage: " << compression_percentage << "%" << endl;

    destroyAllWindows();

    return 0;
}
