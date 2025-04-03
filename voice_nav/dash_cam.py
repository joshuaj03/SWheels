import subprocess
import os 
import signal
import atexit
import multiprocessing

stream_process = None

def run_stream():
    process = subprocess.Popen([
        "mjpg_streamer", 
        "-i", "input_uvc.so -f 30 -r 1280x720", 
        "-o", "output_http.so -p 8080 -w /usr/share/mjpeg-streamer/www"
    ], stdout=subprocess.PIPE, stderr=subprocess.PIPE)

    print("🚀 Dash cam streaming started...!")
    process.wait()  # Wait for the process to run

def start_stream():
    global stream_process
    stop_existing_stream()
    
    if stream_process is None or not stream_process.is_alive():
        stream_process = multiprocessing.Process(target=run_stream, daemon=True)
        stream_process.start()
        print(f"🔄 Stream process started with PID: {stream_process.pid}")

def stop_existing_stream():
    global stream_process
    if stream_process is not None:
        stream_process.terminate()  # Stop the process
        stream_process.join()
        stream_process = None
        print("⛔ Previous stream stopped.")

    # Ensure no other mjpg_streamer process is running
    subprocess.run(["pkill", "-9", "-f", "mjpg_streamer"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)

def stop_stream():
    stop_existing_stream()
    print("🚫 Dash cam streaming stopped!")

atexit.register(stop_stream)

if __name__ == "__main__":
    start_stream()
