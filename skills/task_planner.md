# Skill: Multi-Step Task Planning & Decomposition
Keywords: plan, steps, complex, calculate, search, save, solve, summary, workflow

## Mục đích
Hướng dẫn Agent phân rã các nhiệm vụ phức tạp, nhiều bước (multi-step workflow) thành chuỗi hành động tuần tự, tránh nhầm lẫn logic và giảm thiểu vòng lặp vô tận.

## Quy tắc thực hiện (Guidelines)
1. **Phân tích yêu cầu (Decomposition):**
   - Đọc kỹ yêu cầu người dùng để xác định các nhiệm vụ con (Sub-tasks).
   - Xác định rõ thứ tự phụ thuộc: Bước nào cần làm trước để lấy dữ liệu đầu vào cho bước sau?

2. **Quy trình chuẩn mẫu (Standard Workflow):**
   - **Bước 1 (Thu thập/Tính toán):** Gọi `calculator`, `web_search`, hoặc `read_file` để lấy dữ liệu thô.
   - **Bước 2 (Ghi nhớ/Lưu trữ):** Nếu cần lưu trữ lâu dài hoặc tái sử dụng, gọi `memory_save` hoặc `file (write)`.
   - **Bước 3 (Tổng hợp & Trả lời):** Sau khi các tool hoàn tất thành công, tổng hợp thông tin và trả về kết quả cuối cùng qua `final_answer`.

3. **Ví dụ thực thi (ReAct Example):**
   - *User:* "Tìm thủ đô của Pháp rồi ghi vào file capital.txt"
   - *Thought:* Tôi cần tìm thủ đô của nước Pháp bằng công cụ web_search trước.
   - *Action:* tool_call
   - *Action Input:* {"tool": "web_search", "args": {"query": "capital of France"}}
   - *(Nhận Observation: Paris)*
   - *Thought:* Đã có kết quả là Paris, giờ tôi ghi vào file capital.txt.
   - *Action:* tool_call
   - *Action Input:* {"tool": "file", "args": {"action": "write", "path": "capital.txt", "content": "Paris"}}
   - *(Nhận Observation: Write successfully)*
   - *Thought:* Đã ghi file thành công. Tôi sẽ hoàn thành nhiệm vụ.
   - *Action:* final_answer
   - *Action Input:* Đã tìm được thủ đô nước Pháp là Paris và ghi vào tệp capital.txt thành công.