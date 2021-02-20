#include<iostream>
#include<vector>
#include <sstream>
#include <string>
#include<queue>
#include<algorithm>
using namespace std;

//Functions
void splitBySpace(vector<string> &data,string text)
{
	stringstream stringStream(text);
	while(stringStream.good())
	{
		string substr;
		getline(stringStream, substr, ' ');
		data.push_back(substr);
	}
}

class Car;
class Worker
{
public:
	//Constructor
	Worker(int time_coeff);

	//If time_left is less than 0 then return 1
	//1 means the job is done
	bool decrease_time_left(int time)
	{
		if(time_left > 0)
		{
			time_left -= time;
			return 1;
		}
		return 0;
	}

	bool is_done()
	{
		if(time_left <= 0) return true;
		return false;
	}

	int get_time_left() {return time_left;}
	void set_time_left();
	Car* get_currently_washing() {return currently_washing;}
	void set_currently_washing(Car *car) {currently_washing = car;}
	int get_time_coefficient() {return time_coefficient;}
	void set_working_status(bool status) {is_working = status;}
	int get_worker_id() {return worker_id;}
	void set_worker_id(int id) {worker_id = id;}
	bool is_occupied() {return is_working;}
	bool set_is_working(bool status) {is_working = status;}
private:
	int time_coefficient;
	Car *currently_washing;
	bool is_working;
	int time_left;
	int worker_id;
};

Worker::Worker(int time_coeff)
{
	time_coefficient = time_coeff;
	is_working = false;
	time_left = 0;
}

class Car
{
public:
	//Constructor
	Car(int _luxury_coeff,int _car_id);

	int get_car_id() {return car_id;}
	void show_car_info();
	int get_luxury_coefficient();

private:
	int car_id;
	int luxury_coefficient;
};
Car::Car(int _luxury_coeff,int _car_id)
{
	luxury_coefficient = _luxury_coeff;

	car_id = _car_id;
}

int Car::get_luxury_coefficient() {return luxury_coefficient;}
void Worker::set_time_left() {time_left =  time_coefficient * currently_washing->get_luxury_coefficient();}

void Car::show_car_info()
{
	cout << "Luxury coefficient: " << luxury_coefficient << endl;
}

class Stage
{
public:
	void add_worker(Worker *newWorker);
	void add_to_queue(Car *newCar);
	void show_workers_info();
	void show_waiting_cars(bool label = true,bool start = false);
	int get_waiting_cars_size(){return waitingCars.size();}
	vector<Worker*> get_workers() {return workers;}
	Worker* get_worker(int index){return workers[index];}
	Car* get_waiting_car()
	{
		Car *front = waitingCars.front();
		waitingCars.pop();
		return front;
	}

private:
	vector<Worker*> workers;
	queue<Car*> waitingCars;
};

void Stage::add_to_queue(Car *newCar)
{
	waitingCars.push(newCar);
}

void Stage::add_worker(Worker *newWorker)
{
	workers.push_back(newWorker);
}

void Stage::show_workers_info()
{
	for(int i = 0;i < workers.size();i++)
	{
		cout << "Worker ID: " << workers[i]->get_worker_id() << endl;
		if(workers[i]->is_occupied())
		{
			Car *car = workers[i]->get_currently_washing();

			if(car->get_car_id() != -1)
			{
				cout << "Car ID: " << car->get_car_id()  << endl;
				car->show_car_info();
				cout << "Time left: " << workers[i]->get_time_left() << endl;
			}
			
		}
		else cout << "Free" << endl;
	}
}

void Stage::show_waiting_cars(bool label,bool start)
{
	queue<Car*> copy = waitingCars;

	if(label) cout << "Cars in waiting queue: " << endl;
	if(!start)
	{
		while(copy.size() > 0)
		{
			cout << "Car ID: " << copy.front()->get_car_id() << endl;

			cout << "Luxury coefficient: " << copy.front()->get_luxury_coefficient() << endl;
			copy.pop();
		}
	}
}

class CarWash
{
public:
	//Constructor
	CarWash();

private:
	vector<Stage*> stages;
	vector<Car*> finished;
	int total_time_passed = 0;
	int _advance_time = 0;
	int total_cars;
	int total_workers = 0;

	void addStage(vector<string> &data);
	void addCar(vector<string> &data);
	void advance_time();
	void assign_work(vector<int> unoccupied_workers_id,int stageId);
	void do_work(vector<int> occupied_workers_id,int stageId);
	void move_stage(Car *car,int stageNo);
	void show_stage_info(int index);
	void workers_status(int stage_no,vector<int> &unoccupied_worker_id,vector<int> &occupied_worker_id);
	void show_finished_cars();
	void show_all_waiting_cars();
	void show_carwash_info();
	void finish();
	void set_advance_time(int time)
	{
		total_time_passed += time;
		_advance_time = time;
	}
	bool is_finished()
	{
		if(total_cars == finished.size()) return true;
		return false;
	}
};


CarWash::CarWash()
{
	total_cars = 0;
	string command;

	vector<string> data;
	while(getline(cin,command))
	{
		splitBySpace(data,command);

		if(data[0] == "add_stage")
		{
			addStage(data);
			cout << "OK" << endl;
		}

		else if(data[0] == "add_car")
		{
			addCar(data);
			total_cars++;
			cout << "OK" << endl;
		}

		else if(data[0] == "advance_time")
		{
			set_advance_time(stoi(data[1]));
			advance_time();
			cout << "OK" << endl;
		}

		else if(data[0] == "show_stage_info")
		{
			show_stage_info(stoi(data[1]));
		}

		else if(data[0] == "show_carwash_info")
		{
			show_carwash_info();
		}

		else if(data[0] == "finish")
		{
			cout << "OK" << endl;
			finish();
		}

		else if(data[0] == "show")
		{
			for(int i = 0;i < stages.size();i++)
			{
				show_stage_info(i);
			}
		}

		//else cout << "Wrong Command" << endl;

		data.clear();

	}
}

void CarWash::addStage(vector<string> &data)
{
	Stage *new_stage;

	new_stage = new Stage();

	for(int i = 2; i < data.size();i++)
	{
		Worker *new_worker = new Worker(stoi(data[i]));
		new_worker->set_worker_id(total_workers);
		new_stage->add_worker(new_worker);
		total_workers++;
	}

	stages.push_back(new_stage);
}

void CarWash::addCar(vector<string> &data)
{
	if(stages.size() == 0) 
		cout << "Please add a stage first!" << endl;

	else
	{
		int car_id = stages[0]->get_waiting_cars_size();
		int luxury_coefficient = stoi(data[1]);
		stages[0]->add_to_queue(new Car(luxury_coefficient, car_id));
	}
}

void CarWash::advance_time()
{
	for(int time_start = 1; time_start <= _advance_time;time_start++)
	{
		for(int stageId = 0;stageId < stages.size();stageId++)
		{
			vector<int> unoccupied_workers_id;
			vector<int> occupied_workers_id;
			workers_status(stageId,unoccupied_workers_id,occupied_workers_id);

			if(occupied_workers_id.size() > 0) do_work(occupied_workers_id,stageId);
			if(unoccupied_workers_id.size() > 0) assign_work(unoccupied_workers_id,stageId);
		}
	}
}

void CarWash::assign_work(vector<int> unoccupied_workers_id,int stageId)
{
	sort(unoccupied_workers_id.begin(), unoccupied_workers_id.end());
	int waiting_cars_size = stages[stageId]->get_waiting_cars_size();
	int number_of_unoccupied = unoccupied_workers_id.size();

	if(number_of_unoccupied > 0 && waiting_cars_size > 0)
	{
		Worker* worker = stages[stageId]->get_worker(unoccupied_workers_id[0]);
		Car* car = stages[stageId]->get_waiting_car();

		worker->set_working_status(true);
		worker->set_currently_washing(car);
		worker->set_time_left();
	}
}

void CarWash::do_work(vector<int> occupied_workers_id,int stageId)
{
	sort(occupied_workers_id.begin(), occupied_workers_id.end());

	bool move = false;
	for(int i = 0;i < occupied_workers_id.size();i++)
	{
		int worker_id = occupied_workers_id[i];
		Worker* worker = stages[stageId]->get_worker(worker_id);

		int time_left_previous = worker->get_time_left();
		worker->decrease_time_left(1);
		bool is_done = worker->is_done();


		if(is_done && !move && time_left_previous < 1)
		{
			move = true;
			move_stage(worker->get_currently_washing(),stageId);
			worker->set_working_status(false);

			vector<int> unoccupied_workers_id;
			unoccupied_workers_id.push_back(worker_id);
			assign_work(unoccupied_workers_id,stageId);
		}
	}
}

void CarWash::move_stage(Car *car,int stageNo)
{
	//Finished
	if(stageNo == stages.size() - 1)
	{
		finished.push_back(car);
	}
	//Move to another stage
	else
	{
		stages[stageNo + 1]->add_to_queue(car);
	}
}

void CarWash::show_stage_info(int index)
{
	cout << "Stage ID: " << index << endl;
	stages[index]->show_workers_info();

	if(total_time_passed == 0) stages[index]->show_waiting_cars(true,true);
	else stages[index]->show_waiting_cars();
}

void CarWash::workers_status(int stage_no,vector<int> &unoccupied_worker_id,vector<int> &occupied_worker_id)
{
	vector<Worker*> workers_of_stage = stages[stage_no]->get_workers();
	for(int i = 0;i < workers_of_stage.size();i++)
	{
		if(!workers_of_stage[i]->is_occupied()) unoccupied_worker_id.push_back(i);
		else occupied_worker_id.push_back(i);
	}
}

void CarWash::show_finished_cars()
{
	for(int i = 0;i< finished.size();i++)
	{
		cout << "Car ID: " <<finished[i]->get_car_id() << endl;
		cout << "Luxury coefficient: " <<finished[i]->get_luxury_coefficient() << endl;
	}
}

void CarWash::show_all_waiting_cars()
{
	for(int i = 0;i < stages.size();i++)
	{
		stages[i]->show_waiting_cars(false);
	}
}

void CarWash::show_carwash_info()
{
	cout << "Passed time: " << total_time_passed << endl;
	cout << "Cars waiting: " << endl;
	show_all_waiting_cars();
	cout << "Stages info: " << endl;
	for(int i = 0;i < stages.size();i++)
	{
		show_stage_info(i);
	}
	cout << "Cars finished: " << endl;
	show_finished_cars();
}

void CarWash::finish()
{
	while(!is_finished())
	{
		set_advance_time(1);
		advance_time();
	}
}

int main()
{
	CarWash pipeline;
	return 0;
}